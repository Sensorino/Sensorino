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
#include "RuleService.h"
#include "FragmentedDatagram.h"

/* TODO: make these configurable */
#define CONFIG_CSN_PIN  10
#define CONFIG_INTR_PIN 14
#define CONFIG_CE_PIN   15

static FragmentedDatagram<RHReliableDatagram, RH_NRF24_MAX_MESSAGE_LEN,
        MAX_MESSAGE_SIZE> *radioManager;

Sensorino::Sensorino(bool noSM, bool noRE) {
    if (sensorino)
        die(PSTR("For now only one allowed"));

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
    radioManager = new FragmentedDatagram<RHReliableDatagram,
            RH_NRF24_MAX_MESSAGE_LEN, MAX_MESSAGE_SIZE>(*radio, address);

    servicesNum = 0;

    sensorino = this;

    /* Initialise the radio and put it in Rx mode */
    radioManager->init();
    radioOpDone();

    /* Create a Service Manager unless we were told not to */
    if (!noSM)
        new ServiceManagerService();

    /* Create a Rule Engine unless we were told not to */
    ruleEngine = NULL;
    if (!noRE)
        ruleEngine = new RuleService();

    PCMSK0 = 0;
    PCMSK1 = 0;
    PCMSK2 = 0;
    PCICR = 0;

    /* Ready to go.  Interrupt is active-low.  */
    pinMode(CONFIG_INTR_PIN, INPUT);
    attachGPIOInterrupt(CONFIG_INTR_PIN, LEVEL_LOW, radioInterrupt);

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
    Message msg;
    uint8_t len;

    radioBusy++;
    if (radioManager->recvfromAck(msg.getWriteBuffer(), &len,
                NULL, NULL, NULL, NULL)) {
        msg.writeLength(len);
        handleMessage(msg);
    }
    radioBusy--;
}

void Sensorino::radioInterrupt(uint8_t pin) {
    /* Check if the radio is busy, e.g. this may be a Tx FIFO empty
     * interrupt or, the ACK receival, and we're not interested in
     * those, RadioHead should take care of them.
     */
    if (radioBusy)
        return;

    /* This will handle new messages */
    sensorino->radioOpDone();
}

volatile uint8_t Sensorino::radioBusy = 0;

bool Sensorino::sendMessage(Message &m) {
    bool ret;
    uint8_t dest = m.getDstAddress();

    /* Note if we were to ignore messages addressed at ourselves, such as
     * responses to events, we can do a m.getDstAddress() != getAddress()
     * check here.  Currently we want the Base to see such messages though.
     */
    if (dest == getAddress())
        dest = getBaseAddress();

    radioBusy++;
    ret = radioManager->sendtoWait((uint8_t *) m.getRawData(),
            m.getRawLength(), dest);

    if (m.getType() == Message::PUBLISH && ruleEngine)
        ruleEngine->evalPublish(m); /* Note: may recurse */
    radioBusy--;

    if (!radioBusy)
        radioOpDone(); /* Note: also may recurse (should use bottom halves) */

    return ret;
}

void Sensorino::handleMessage(Message &msg) {
    int svcId;
    Service *targetSvc = NULL;

    if (msg.find(SERVICE_ID, 0, &svcId))
        targetSvc = getServiceById(svcId);

    if (!targetSvc) {
        Message err(getAddress(), getBaseAddress());
        err.setType(Message::ERR);
        err.addDataTypeValue(Data::SERVICE_ID);
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
        die(PSTR("MAX_SERVICES reached"));

    if (getServiceById(s->getId()))
        die(PSTR("Duplicate service ID"));

    services[servicesNum++] = s;
}

void Sensorino::deleteService(Service *s) {
    uint8_t i;

    for (i = 0; i < servicesNum; i++)
        if (s == services[i])
            break;

    if (i >= servicesNum)
        die(PSTR("deleteService: not found"));

    servicesNum--;
    for (; i < servicesNum; i++)
        services[i] = services[i + 1];
}

Service *Sensorino::getServiceById(int id) {
    for (uint8_t i = 0; i < servicesNum; i++)
        if (services[i]->getId() == id)
            return services[i];

    return NULL;
}

Service *Sensorino::getServiceByNum(uint8_t num) {
    return num < servicesNum ? services[num] : NULL;
}

void Sensorino::die(const prog_char *err) {
    cli();
    Serial.begin(115200);
    pgmWrite(Serial, PSTR("Panic"));
    if (err) {
        pgmWrite(Serial, PSTR(" because: "));
        pgmWrite(Serial, err);
    }
    pgmWrite(Serial, PSTR("\nStopping\n"));
    /* TODO: also broadcast the same stuff over all radio channels, etc.? */
    #if (RH_PLATFORM == RH_PLATFORM_SIMULATOR)
        exit(2);
    #endif

    while (1);
}

#if (RH_PLATFORM != RH_PLATFORM_SIMULATOR)
ISR(BADISR_vect) {
    Sensorino::die(PSTR("BADISR"));
}

/* The numbers of ports and pins below are model specific */
#ifndef __AVR_ATmega328P__
# error Only 328P supported for now
#endif

/* Globals.. can be moved to Sensorino as statics */
static volatile void *gpio_handler[NUM_DIGITAL_PINS];
static volatile uint8_t port_obj_mask[3], port_edge_mask[3];
static volatile uint8_t port_val[3];
/* Could get rid of this at some cost... */
static volatile uint8_t pcint_to_gpio[24] = {
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
};

static void SensorinoGPIOISR(uint8_t port, volatile uint8_t *pin_reg) {
    uint8_t pin_msk = 1;
    uint8_t num;

    uint8_t new_val = *pin_reg;
    uint8_t diff = port_val[port] ^ new_val;
    port_val[port] ^= diff & port_edge_mask[port];

    for (volatile uint8_t *gpio = &pcint_to_gpio[port << 3]; diff;
            pin_msk <<= 1, gpio++) {
        if (!(diff & pin_msk))
            continue;
        diff ^= pin_msk;

        uint8_t num = *gpio;
        if (num == 0xff)
            continue;

        /* For edge-triggered pins call back once.  Some edges may
         * be missed (any number of opposite pairs) but we have no way to
         * detect edges other than by comparing latest value because the
         * PCIFR flag has been cleared by now.
         *
         * For level-triggered loop as long as level is maintained.  No
         * need to re-check after re-enabling interrupts because a new
         * edge will set PCIFR and send us back here as soon as we return.
         * Note that at this point we already know that the level matches.
         */
        while (1) {
            if (port_obj_mask[port] & pin_msk) {
                GenIntrCallback *cb = (GenIntrCallback *) gpio_handler[num];
                cb->call(num);
            } else {
                void (*handler)(uint8_t) =
                    (void (*)(uint8_t)) gpio_handler[num];
                handler(num);
            }

            if (port_edge_mask[port] & pin_msk)
                break;
            if ((new_val ^ *pin_reg) & pin_msk)
                break;
        }
    }
}

ISR(PCINT0_vect) {
    SensorinoGPIOISR(0, &PINB);
}

ISR(PCINT1_vect) {
    SensorinoGPIOISR(1, &PINC);
}

ISR(PCINT2_vect) {
    SensorinoGPIOISR(2, &PIND);
}

static void doAttachGPIOInterrupt(uint8_t pin, uint8_t trigger,
        void *handler, uint8_t obj) {
    if (pin >= NUM_DIGITAL_PINS)
        Sensorino::die(PSTR("Bad pin number"));

    uint8_t port = digitalPinToPCICRbit(pin);
    uint8_t pcint = digitalPinToPCMSKbit(pin);
    uint8_t pcmsk = 1 << pcint;

    gpio_handler[pin] = handler;
    pcint_to_gpio[(port << 3) | pcint] = pin;
    if (obj)
        port_obj_mask[port] |= pcmsk;
    else
        port_obj_mask[port] &= ~pcmsk;

    port_val[port] &= ~pcmsk;
    if (trigger == Sensorino::EDGE_ANY) {
        port_edge_mask[port] |= pcmsk;

        port_val[port] |= pcmsk & *portInputRegister(digitalPinToPort(pin));
    } else {
        port_edge_mask[port] &= ~pcmsk;

        if (trigger == Sensorino::LEVEL_LOW)
            port_val[port] |= pcmsk;
    }

    /* Enable corresponding interrupt */
    *digitalPinToPCMSK(pin) |= pcmsk;
    *digitalPinToPCICR(pin) |= 1 << port;
}
#else
static void doAttachGPIOInterrupt(uint8_t pin, uint8_t trigger,
        void *handler, uint8_t obj) {}
#endif

void Sensorino::attachGPIOInterrupt(uint8_t pin, uint8_t trigger,
        void (*handler)(uint8_t pin)) {
    doAttachGPIOInterrupt(pin, trigger, (void *) handler, 0);
}

void Sensorino::attachGPIOInterrupt(uint8_t pin, uint8_t trigger,
        GenIntrCallback *callback) {
    doAttachGPIOInterrupt(pin, trigger, callback, 1);
}

/* Potentially-temporary global single sensorino instance.  We can pass this
 * pointer around when calling service constructors later, for the moment
 * let's use a global...
 */
Sensorino *sensorino;

/* vim: set sw=4 ts=4 et: */
