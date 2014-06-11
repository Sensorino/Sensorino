#include <Arduino.h>
#include "mini-radiohead.h"
//#include <RHReliableDatagram.h>
#if (RH_PLATFORM == RH_PLATFORM_SIMULATOR) 
 // Simulate the sketch on Linux
 #include <MockArduino.h>
 #include <RHutil/simulator.h>
 #include <Dummy.h>
#else
 //#include <RH_NRF24.h>
#endif

#if (RH_PLATFORM == RH_PLATFORM_SIMULATOR)
#define cli()
#define sei()
#endif

#if (RH_PLATFORM == RH_PLATFORM_SIMULATOR)
  #define MAX_MESSAGE_LEN 28
#else
  #define MAX_MESSAGE_LEN RH_NRF24_MAX_MESSAGE_LEN
#endif

#include "Sensorino.h"
#include "Message.h"
#include "Service.h"
#include "ServiceManagerService.h"

/* TODO: make these configurable */
#define CONFIG_CSN_PIN  10
#define CONFIG_INTR_PIN 14
#define CONFIG_CE_PIN   15

Sensorino::Sensorino(int noSM) {
    if (sensorino)
        Sensorino::die("For now only one allowed");

    /* TODO: make the constructor accept a Config object, which will
     * provide default config, including default address and some service
     * creation, based on whatever is saved in EEPROM (EEPROMConfig is a
     * subclass of Config).
     */

    // Class to manage message delivery and receipt, using the driver declared above
    #if (RH_PLATFORM == RH_PLATFORM_SIMULATOR)
    RHGenericDriver *radio = new Dummy();
    #else
    RHGenericDriver *radio = new RH_NRF24(CONFIG_CE_PIN, CONFIG_CSN_PIN);
    #endif
    radioManager = new RHReliableDatagram(*radio, address);

    servicesNum = 0;

    sensorino = this;

    /* Initialise the radio and put it in Rx mode */
    radioManager->init();
    radioOpDone();

    /* Create a Service Manager unless we were told not to */
    if (!noSM)
        new ServiceManagerService();

    PCMSK0 = 0;
    PCMSK1 = 0;
    PCMSK2 = 0;
    PCICR = 0;

    /* Ready to go */
    pinMode(CONFIG_INTR_PIN, INPUT);
    attachGPIOInterrupt(CONFIG_INTR_PIN, radioInterrupt, NULL);

    sei();
}

/* Any time we interact with the NRF24L01+, once the radio becomes idle,
 * this function must be called.  This means that we need to either use the
 * blocking variants of functions (sendtoWait instead of sendto) or
 * set up the interrupt handler to detect the end of the operation
 * condition.
 * The NRF24L01+ must be idle (e.g. not transmitting an ACK or anything)
 * when this is called, so that we can go back to RX mode.
 */
void Sensorino::radioOpDone(void) {
    /* A call to either .available() or any .recv* function switches
     * the radio back to Rx mode if not already enabled.  But, any
     * of these functions will also download a new packet from the
     * radio if available, seems there's no clean way in
     * RH(Reliable)Datagram to just enable RX.  This means that any
     * interrupt would get cleared so if the function returns true, we
     * must handle the packet now or it may be lost later.  In other
     * words, we need to wait for .available() to return false.
     */
    while (radioManager->available())
        radioCheckPacket();
}

/* TODO: call this in a Bottom Half */
void Sensorino::radioCheckPacket(void) {
    uint8_t pkt[MAX_MESSAGE_LEN], len;

    radioBusy++;
    if (radioManager->recvfromAck(pkt, &len, NULL, NULL, NULL, NULL))
        handleMessage(pkt, len);
    radioBusy--;
}

void Sensorino::radioInterrupt(int pin, void *s) {
    /* Make sure the line is low since we're probably registered for
     * both edges.  The NRF24L01+ interrupt is active-low.
     */
    if (digitalRead(CONFIG_INTR_PIN))
        return;

    /* Check if the radio is busy, e.g. this may be a Tx FIFO empty
     * interrupt or, the ACK receival, and we're not interested in
     * those, RadioHead should take care of them.
     */
    if (radioBusy)
        return;

    /* This will handle new messages */
    sensorino->radioOpDone();
}

#if (RH_PLATFORM == RH_PLATFORM_SIMULATOR)
 volatile uint8_t Sensorino::radioBusy = 0;
#else
 volatile bool Sensorino::radioBusy = 0;
#endif


bool Sensorino::sendMessage(Message &m) {
    bool ret;

    radioBusy++;
    ret = radioManager->sendtoWait((uint8_t *) m.getRawData(),
            m.getRawLength(), m.getDstAddress());
    radioBusy--;

    radioOpDone();
    return ret;
}

void Sensorino::handleMessage(const uint8_t *rawData, int len) {
    Message msg(rawData, len);
    int svcId;
    Service *targetSvc = NULL;

    if (msg.find(SERVICE_ID, 0, &svcId))
        targetSvc = getServiceById(svcId);

    if (!targetSvc) {
        Message err(getAddress(), getBaseAddress());
        err.setType(ERR);
        sendMessage(err);
        return;
    }

    targetSvc->handleMessage(&msg);
}

void Sensorino::setAddress(uint8_t address) {
    Sensorino::address = address;

    radioManager->setThisAddress(address);
}

uint8_t Sensorino::getAddress() {
    return address;
}

void Sensorino::addService(Service *s) {
    if (servicesNum >= MAX_SERVICES)
        die("MAX_SERVICES reached");

    if (getServiceById(s->getId()))
        die("Duplicate service ID");

    services[servicesNum++] = s;
}

void Sensorino::deleteService(Service *s) {
    int i;

    for (i = 0; i < servicesNum; i++)
        if (s == services[i])
            break;

    if (i >= servicesNum)
        die("deleteService: not found");

    servicesNum--;
    for (; i < servicesNum; i++)
        services[i] = services[i + 1];
}

Service *Sensorino::getServiceById(int id) {
    for (int i = 0; i < servicesNum; i++)
        if (services[i]->getId() == id)
            return services[i];

    return NULL;
}

Service *Sensorino::getServiceByNum(int num) {
    return num < servicesNum ? services[num] : NULL;
}

void Sensorino::die(const char *err) {
    cli();
    Serial.begin(115200);
    Serial.write("Panic");
    if (err) {
        Serial.write(" because: ");
        Serial.write(err);
    }
    Serial.write("\nStopping\n");
    /* TODO: also broadcast the same stuff over all radio channels, etc.? */
    #if (RH_PLATFORM == RH_PLATFORM_SIMULATOR)
        exit(2);
    #endif

    while (1);
}

#if (RH_PLATFORM != RH_PLATFORM_SIMULATOR)

/* The numbers of ports and pins below are model specific */
#ifndef __AVR_ATmega328P__
# error Only 328P supported for now
#endif

/* Globals.. can be moved to Sensorino as statics */
static void (*gpio_handler[NUM_DIGITAL_PINS])(int pin, void *data);
static void *gpio_data[NUM_DIGITAL_PINS];
static volatile uint8_t port_val[3];
/* Could get rid of this at some cost... */
static volatile uint8_t pcint_to_gpio[24] = {
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
};

static void SensorinoGPIOISR(int port, uint8_t new_val) {
    uint8_t diff = port_val[port] ^ new_val;
    port_val[port] = new_val;

    for (volatile uint8_t *gpio = &pcint_to_gpio[port << 3]; diff;
            diff >>= 1, gpio++)
        if ((diff & 1) && ~*gpio)
            gpio_handler[*gpio](*gpio, gpio_data[*gpio]);
}

ISR(PCINT0_vect) {
    SensorinoGPIOISR(0, PINB);
}

ISR(PCINT1_vect) {
    SensorinoGPIOISR(1, PINC);
}

ISR(PCINT2_vect) {
    SensorinoGPIOISR(2, PIND);
}

void Sensorino::attachGPIOInterrupt(int pin,
        void (*handler)(int pin, void *data), void *data) {
    if (pin >= NUM_DIGITAL_PINS)
        die("Bad pin number");

    int pcint = (digitalPinToPCICRbit(pin) << 3) | digitalPinToPCMSKbit(pin);

    gpio_handler[pin] = handler;
    gpio_data[pin] = data;
    pcint_to_gpio[pcint] = pin;

    /* Enable corresponding interrupt */
    port_val[digitalPinToPCICRbit(pin)] =
        *portInputRegister(digitalPinToPort(pin));
    *digitalPinToPCMSK(pin) |= 1 << digitalPinToPCMSKbit(pin);
    *digitalPinToPCICR(pin) |= 1 << digitalPinToPCICRbit(pin);
}
#else
void Sensorino::attachGPIOInterrupt(int pin,
        void (*handler)(int pin, void *data), void *data) {}
#endif

/* Potentially-temporary global single sensorino instance.  We can pass this
 * pointer around when calling service constructors later, for the moment
 * let's use a global...
 */
Sensorino *sensorino;

/* vim: set sw=4 ts=4 et: */
