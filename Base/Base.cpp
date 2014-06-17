//#include <RHReliableDatagram.h>
//#include <RH_NRF24.h>
#include "mini-radiohead.h"

#include "Base.h"
#include "MessageJsonConverter.h"
#include "FragmentedDatagram.h"

/* TODO: make these configurable */
#define CONFIG_CSN_PIN  10
#define CONFIG_INTR_PIN 14
#define CONFIG_CE_PIN   15

#if (CONFIG_INTR_PIN == SS)
# error Please rewire your nRF24 interrupt to a dfferent pin
#endif

/* TODO: put all the utils in a header */
#ifndef unlikely
# define unlikely(x)	__builtin_expect((x), 0)
#endif

void watchdogConfig(uint8_t x) {
    WDTCSR = _BV(WDCE) | _BV(WDE);
    WDTCSR = x;
}

static MessageJsonConverter conv;

static RH_NRF24 radio(CONFIG_CE_PIN, CONFIG_CSN_PIN);
static FragmentedDatagram<RHReliableDatagram, RH_NRF24_MAX_MESSAGE_LEN,
        MAX_MESSAGE_SIZE> radioManager(radio, 0);

void Base::setup() {
    watchdogConfig(0);

    Serial.begin(115200);

    radioManager.init();

    PCMSK0 = 0;
    PCMSK1 = 0;
    PCMSK2 = 0;
    PCICR = 0;

    pinMode(CONFIG_INTR_PIN, INPUT);
    *digitalPinToPCMSK(CONFIG_INTR_PIN) |=
        1 << digitalPinToPCMSKbit(CONFIG_INTR_PIN);
    *digitalPinToPCICR(CONFIG_INTR_PIN) |=
        1 << digitalPinToPCICRbit(CONFIG_INTR_PIN);

    sei();
}

static bool checkJsonError(aJsonObject *obj) {
    if (aJson.getObjectItem(obj, "type"))
        return 0;

    if (aJson.getObjectItem(obj, "error")) {
        char *str = aJson.print(obj);
        Serial.write(str);
        free(str);
        return 1;
    }

    Serial.write("{\"error\":\"noType\"}");
    return 1;
}

void Base::loop() {
    static aJsonStream aJsonSerial(&Serial);

    /* Wait until something happens on UART or radio */
    __asm__ volatile ("sleep");

    while (Serial.available()) {
        conv.putch(Serial.read());

        /* We've received and parsed a new JSON message */
        if (conv.obj) {
            Message *msg = NULL;
            bool jsonOk = !checkJsonError(conv.obj);

            if (jsonOk)
                msg = MessageJsonConverter::jsonToMessage(*conv.obj);
            aJson.deleteItem(conv.obj);
            conv.obj = NULL;

            /* Succesfully converted to Message */
            if (msg) {
                if (!radioManager.sendtoWait((uint8_t *) msg->getRawData(),
                        msg->getRawLength(), msg->getDstAddress()))
                    Serial.write("{\"error\":\"xmitError\"}");
                delete msg;
            } else if (jsonOk)
                Serial.write("{\"error\":\"structError\"}");
        }
    }

    while (radioManager.available()) {
        uint8_t pkt[RH_NRF24_MAX_MESSAGE_LEN], len;

        /* New radio packet received */
        if (radioManager.recvfromAck(pkt, &len, NULL, NULL, NULL, NULL)) {
            Message msg(pkt, len);
            aJsonObject *obj = MessageJsonConverter::messageToJson(msg);

            /* Successfully converted to JSON */
            if (obj) {
                /* FIXME this blocks */
                aJson.print(obj, &aJsonSerial);
                aJson.deleteItem(obj);
            }
        }
    }
}

#if 0
ISR(USART_RX_vect) {
    uint8_t status = UCSR0A;
    uint8_t ch = UDR0;

    if (unlikely(status & 0x14))
        return;

    conv.putch(ch);
}
#endif

EMPTY_INTERRUPT(PCINT0_vect);
ISR(PCINT1_vect, ISR_ALIASOF(PCINT0_vect));
ISR(PCINT2_vect, ISR_ALIASOF(PCINT0_vect));

#include "../Sensorino/Sensorino.h"

void Sensorino::die(const prog_char *err) {
    cli();
    Serial.begin(115200);
#define pgmWrite(stream, string) \
    { \
        char buf[strlen_P(string) + 1]; \
        strcpy_P(buf, string); \
        stream.write(buf); \
    }
    pgmWrite(Serial, PSTR("Panic"));
    if (err) {
        pgmWrite(Serial, PSTR(" because: "));
        pgmWrite(Serial, err);
    }
    pgmWrite(Serial, PSTR("\nStopping\n"));
    /* TODO: also broadcast the same stuff over all radio channels, etc.? */
    while (1);
}

/* vim: set sw=4 ts=4 et: */
