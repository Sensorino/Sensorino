#ifndef SENSORINO_H
#define SENSORINO_H

#include <stdint.h>
#include <stddef.h>
#include <avr/pgmspace.h>

// Maximum number of instantiable base services
#define MAX_SERVICES 20

class Service;
class Message;
class RuleService;

class GenIntrCallback {
public:
	virtual void call(int pin) = 0;
};

class Sensorino {
    public:
        Sensorino(bool noSM = 0, bool noRE = 0);

        void handleMessage(Message &m);
        void setAddress(uint8_t address);
        uint8_t getAddress();
        uint8_t getBaseAddress() { return 0; };

        bool sendMessage(Message &m);

        void addService(Service *s);
        void deleteService(Service *s);

        Service *getServiceById(int id);
        Service *getServiceByNum(int num);

        static void die(const prog_char *err = NULL) __attribute__((noreturn));

        /* Since the Atmega only has one IRQ line per port (8 GPIOs)
         * this function lets a Service handle individual GPIO interrupts
         * by providing a group handler that calls individual handlers as
         * needed.
         */
        void attachGPIOInterrupt(int pin, void (*handler)(int pin));
        void attachGPIOInterrupt(int pin, GenIntrCallback *callback);

    private:
        uint8_t address;

        Service *services[MAX_SERVICES];
        uint8_t servicesNum;

        RuleService *ruleEngine;

        void radioOpDone(void);
        void radioCheckPacket(void);
        static void radioInterrupt(int pin);

        static volatile uint8_t radioBusy;
};

#define attachObjGPIOInterrupt(pin, method) \
	attachGPIOInterrupt(pin, new IntrCallback<typeof(*this)>(this, &method))

template <typename T>
class IntrCallback : public GenIntrCallback {
	T *obj;
	void (T::*method)(int);
public:
	IntrCallback(T *nobj, void (T::*nmethod)(int)) :
        obj(nobj), method(nmethod) {}
	void call(int pin) { (obj->*method)(pin); }
};

extern Sensorino *sensorino;

#endif // whole file
/* vim: set sw=4 ts=4 et: */
