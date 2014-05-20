#ifndef SENSORINO_H
#define SENSORINO_H

#include <stdint.h>
#include <stddef.h>

// Maximum number of instantiable base services
#define MAX_SERVICES 20

class Service;
class Message;
struct RHReliableDatagram;

class Sensorino
{
    public:
        Sensorino(int noSM = 0);

        void handleMessage(const uint8_t *rawData, int len);
        void setAddress(uint8_t address);
        uint8_t getAddress();
        uint8_t getBaseAddress() { return 0; };

        bool sendMessage(Message &m);

        void addService(Service *s);
        void deleteService(Service *s);

        Service *getServiceById(int id);
        Service *getServiceByNum(int num);

        static void die(const char *err = NULL) __attribute__((noreturn));

        /* TODO:
         * Since the Atmega only has one IRQ line per port (8 GPIOs)
         * this function lets a Service handle individual GPIO interrupts
         * by providing a group handler that calls individual handlers as
         * needed.
         */
        void attachGPIOInterrupt(int pin, void (*handler)(int pin, void *data),
                void *data);

    private:
        uint8_t address;

        Service *services[MAX_SERVICES];
        uint8_t servicesNum;

        RHReliableDatagram *radioManager;

        void radioOpDone(void);
        void radioCheckPacket(void);
        static void radioInterrupt(int pin, void *s);
        static volatile bool radioBusy;
};

extern Sensorino *sensorino;

#endif // whole file
/* vim: set sw=4 ts=4 et: */
