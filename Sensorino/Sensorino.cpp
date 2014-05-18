#include "Sensorino.h"
#include "Message.h"

Sensorino::Sensorino(){
    // Singleton instance of the radio driver
    RH_NRF24 driver;

    // Class to manage message delivery and receipt, using the driver declared above
    manager = new RHReliableDatagram(driver, address);
}

bool Sensorino::sendMessage(Message &m) {
    return manager->sendtoWait(m.getRawData(), m.getRawLength(),
            m.getDstAddress());
}

void Sensorino::onRadioMessage(uint8_t *rawData) {
}

void Sensorino::setAddress(uint8_t address) {
    Sensorino::address = address;
}

uint8_t getAddress() {
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

    serviceNum--;
    for (; i < serviceNum; i++)
        service[i] = service[i + 1];
}

Service *Sensorino::getServiceById(int id) {
    for (int i = 0; i < servicesNum; i++)
        if (service[i]->getId() == id)
            return service[i];

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
    while (1);
}

/* Potnetially-temporary global single sensorino instance.  We can pass this
 * pointer around when calling service constructors later, for the moment
 * let's use a global...
 */
Sensorino *sensorino;

/* vim: set sw=4 ts=4 et: */
