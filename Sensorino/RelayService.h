#include <Arduino.h>

#include "Service.h"

using namespace Data;

class RelayService : public Service {
public:
    RelayService(int id, int pin, bool init_state=0) : Service(id), pin(pin) {
        pinMode(pin, OUTPUT);
        digitalWrite(pin, init_state);

        state = init_state;
    }

protected:
    int pin;
    bool state;

    void onSet(Message *message) {
        bool new_state;

        if (!message->find(SWITCH, 0, &new_state))
            new_state = !state;

        digitalWrite(pin, new_state);
        state = new_state;
    }

    void onRequest(Message *message) {
        Data::Type req;

        if (!message->find(DATATYPE, 0, &req) || req == SWITCH) {
            Message *msg = publish(message);
            msg->addBoolValue(SWITCH, state);
            msg->send();
            return;
        }

        if (req == DATATYPE) {
            /* Send a "service description" -- the type of values that this
             * service publishes and accepts.
             */
            Message *msg = publish();
            msg->addIntValue(COUNT, 0); /* We publish 0 data types */
            msg->addIntValue(COUNT, 1); /* We accept 1 data type */
            msg->addDataTypeValue(SWITCH);
            msg->send();
            return;
        }

        /* We don't understand this request, send an error */
        err(message, DATATYPE)->send();
    }
};
/* vim: set sw=4 ts=4 et: */
