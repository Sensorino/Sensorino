#include <Arduino.h>

#include "Service.h"
#include "Timers.h"

using namespace Data;

#define DEBOUNCE 12 /* Debounce over 12 ms */

class SwitchService : public Service {
public:
    SwitchService(int id, int pin) : Service(id), pin(pin) {
        /* Always enable the pull-up on the switch's GPIO.  If not
         * needed it probably won't hurt, but if this is a wall
         * switch it most likely can only open and close the circuit
         * so we need either a pull-up or a pull-down to properly
         * detect the change.
         * Since the Atmega offers no pull-downs, we always use the
         * pull-up.  This implies that it's best to connect the
         * switch between the GPIO and the Ground.
         */
        pinMode(pin, INPUT_PULLUP);

        sensorino->attachObjGPIOInterrupt(pin, Sensorino::EDGE_ANY,
                SwitchService::pinHandler);
    }

protected:
    int pin;

    void publishSwitch(void) {
        Message *msg = publish();
        msg->addBoolValue(SWITCH, digitalRead(pin) == HIGH);
        msg->send();
    }

    void onRequest(Message *message) {
        Type req;

        /* If the request has no DataType value, or it's asking for the
         * SWITCH value, just send the current switch state.
         */
        if (!message->find(DATATYPE, 0, &req) || req == SWITCH) {
            publishSwitch();
            return;
        }

        if (req == DATATYPE) {
            /* Send a "service description" -- the type of values that this
             * service publishes.
             */
            Message *msg = publish(message);
            msg->addIntValue(COUNT, 1); /* We publish 1 data type */
            msg->addDataTypeValue(SWITCH);
            msg->send();
            return;
        }

        /* We don't understand this request, send an error */
        err(message, DATATYPE)->send();
    }

    void pinHandler(uint8_t pin) {
#ifdef DEBOUNCE
        uint8_t state = digitalRead(pin), count = 0, loops = 0;
        /* Wait until the pin reports the same value in N read()s in a row */
        while (++count < DEBOUNCE) {
            Timers::delay(1);
            if (digitalRead(pin) != state) {
                state = !state;
                loops += count;
                count = 0;
                if (loops >= 255 - DEBOUNCE) {
                    /* Timeout: give up on this pin, must be a clock or
                     * floating, or something else.
                     */
                    sensorino->detachGPIOInterrupt(pin);
                    return;
                }
            }
        }
#endif

        publishSwitch();
    }
};
/* vim: set sw=4 ts=4 et: */
