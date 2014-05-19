#include <RHReliableDatagram.h>
#if (RH_PLATFORM == RH_PLATFORM_SIMULATOR) 
 // Simulate the sketch on Linux
 #include <RHutil/simulator.h>
 #include <Dummy.h>
#else
 #include <RH_NRF24.h>
#endif


#include "Sensorino.h"
#include "Message.h"
#include "Service.h"
#include "ServiceManagerService.h"

Sensorino::Sensorino(int noSM) {
    /* TODO: make the constructor accept a Config object, which will
     * provide default config, including default address and some service
     * creation, based on whatever is saved in EEPROM (EEPROMConfig is a
     * subclass of Config).
     */
    // Class to manage message delivery and receipt, using the driver declared above
    #if (RH_PLATFORM == RH_PLATFORM_SIMULATOR)
    RHGenericDriver *radio = new Dummy();
    #else
    RHGenericDriver *radio = new RH_NRF24();
    #endif
    radioManager = new RHReliableDatagram(*radio, address);

    servicesNum = 0;

    sensorino = this;

    /* Create a Service Manager unless we were told not to */
    if (!noSM)
        new ServiceManagerService();
}

bool Sensorino::sendMessage(Message &m) {
    return radioManager->sendtoWait((uint8_t *) m.getRawData(),
            m.getRawLength(), m.getDstAddress());
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


/* Globals.. can be moved to Sensorino as statics */
#ifndef __AVR_ATmega328P__
#if (RH_PLATFORM == RH_PLATFORM_SIMULATOR)
 // compiling for special case
void cli(){
    // faking
}
#else
# error Only 328P supported for now
#endif
#endif



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
static void (*gpio_handler[NUM_DIGITAL_PINS])(int pin, void *data);
static void *gpio_data[NUM_DIGITAL_PINS];
static uint8_t port_val[3];
/* Could get rid of this at some cost... */
static uint8_t pcint_to_gpio[24] = {
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
};

static void SensorinoGPIOISR(int port, uint8_t new_val) {
    uint8_t diff = port_val[port] ^ new_val;
    port_val[port] = new_val;

    for (uint8_t *gpio = &pcint_to_gpio[port << 3]; diff; diff >>= 1, gpio++)
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

#endif
/* Potentially-temporary global single sensorino instance.  We can pass this
 * pointer around when calling service constructors later, for the moment
 * let's use a global...
 */
Sensorino *sensorino;

/* vim: set sw=4 ts=4 et: */
