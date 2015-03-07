#include "ADCService.h"

using namespace Data;

class OnchipThermometerService : public ADCService {
public:
    /* ADC channel 8, take 64 samples per measurement, every 20 minutes */
    OnchipThermometerService(int id, int8_t offset = 0) :
            ADCService(id, 8, 64, 1200), t_offset(offset) {}

protected:
    int8_t t_offset;

    void publishValue(void) {
        /* Slightly rounded *typical* calibration values from the spec */
        float temp = (prevValue - 0.2883) * 960.0 - t_offset;

        Message *msg = publish();
        msg->addFloatValue(TEMPERATURE, temp);
        msg->send();
    }

    void onRequest(Message *message) {
        Type req;

        /* If the request has no DataType value, or it's asking for the
         * TEMPERATURE value, just send the last temperature measured.
         */
        if (!message->find(DATATYPE, 0, &req) || req == TEMPERATURE) {
            publishValue();
            return;
        }

        if (req == DATATYPE) {
            /* Send a "service description" -- the type of values that this
             * service publishes.
             */
            Message *msg = publish(message);
            msg->addIntValue(COUNT, 1); /* We publish 1 data type */
            msg->addDataTypeValue(TEMPERATURE);
            msg->send();
            return;
        }

        /* We don't understand this request, send an error */
        err(message, DATATYPE)->send();
    }
};
/* vim: set sw=4 ts=4 et: */
