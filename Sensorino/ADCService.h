#include <Arduino.h>

#include "Service.h"
#include "Timers.h"

using namespace Data;

/* TODO: convert constructor parameters to template parameters */
class ADCService : public Service {
public:
    ADCService(int id, uint8_t channel, uint8_t samples, uint16_t interval) :
            Service(id), chan(channel), samplesAvg(samples),
            interval(interval) {
        sampleSum = 0;
        sampleCount = 0;

        if (chan < 6)
            DIDR0 |= 1 << chan;

        /* Start first measurement in 3 seconds */
        Timers::setObjTimeout(ADCService::getSample, 3 * F_TMR);
    }

protected:
    uint8_t chan;
    uint8_t samplesAvg;
    uint16_t interval;
    volatile uint32_t sampleSum;
    volatile uint8_t sampleCount;
    float prevValue;

    virtual void publishValue(void) {
        Message *msg = publish();
        msg->addFloatValue(VOLTAGE, prevValue);
        msg->send();
    }

    virtual void onRequest(Message *message) {
        Type req;

        /* If the request has no DataType value, or it's asking for the
         * VOLTAGE value, just send the last voltage measured.
         */
        if (!message->find(DATATYPE, 0, &req) || req == VOLTAGE) {
            publishValue();
            return;
        }

        if (req == DATATYPE) {
            /* Send a "service description" -- the type of values that this
             * service publishes.
             */
            Message *msg = publish(message);
            msg->addIntValue(COUNT, 1); /* We publish 1 data type */
            msg->addDataTypeValue(VOLTAGE);
            msg->send();
            return;
        }

        /* We don't understand this request, send an error */
        err(message, DATATYPE)->send();
    }

    virtual void processMeasurement(void) {
        /* TODO: skip value if not different from previous measurement by
         * enough and a max period time hasn't passed since last publish.
         */
        prevValue = sampleSum * ((1.1 / 1024 / 16) / samplesAvg);
        publishValue();
    }

    void getSample(void) {
        uint8_t i;

        if (sampleCount == 0)
            /* Schedule next measurement */
            Timers::setObjTimeout(ADCService::getSample,
                    (uint32_t) interval * F_TMR);

        /* For now we busy-wait for the measurement to finish, should just
         * handle the interrupt and continue.  The busy-waiting solves the
         * ADC time-sharing problem for us so multiple ADC Services can
         * work simultaneously.  It also ensures A0-3 digitial outputs
         * are not switched while the ADC is busy which 24.6.2 Analog Noise
         * Cancelling Techniques says is essential.
         */
        /* Since we have long times between samples we disable and enable
         * the ADEN bit repeatedly causing the conversions to take longer.
         * We should check if keeping the ADC on between individual samples
         * in a measurement improves power use.
         */
        PRR &= ~0x01;
        for (i = 0; i < 16; i++) {
            ADMUX = 0xc0 | chan; /* Select channel and 1.1V internal Vref */
            ADCSRA = 0xd7; /* Power ADC on, start, clr ADIF, 1/128 prescaler */
            while (ADCSRA & 0x40); /* Wait for ADSC to go low */
            sampleSum += ADC;
        }
        ADCSRA = 0x10; /* Power ADC off, clear ADIF */
        PRR |= 0x01;

        sampleCount++;
        if (sampleCount >= samplesAvg) {
            /* Publish new value if necessary */
            processMeasurement();

            sampleSum = 0;
            sampleCount = 0;

            return;
        }

        /* Schedule a new sample in 0.5s */
        Timers::setObjTimeout(ADCService::getSample, F_TMR);
    }
};
/* vim: set sw=4 ts=4 et: */
