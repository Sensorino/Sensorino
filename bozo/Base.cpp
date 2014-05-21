#include <RHReliableDatagram.h>
#include <RH_NRF24.h>

#include "Base.h"
#include "MessageJsonConverter.h"

/* TODO: make these configurable */
#define CONFIG_CSN_PIN  14
#define CONFIG_CE_PIN   15
#define CONFIG_INTR_PIN 10

/* TODO: put all the utils in a header */
#ifndef unlikely
# define unlikely(x)	__builtin_expect((x), 0)
#endif

MessageJsonConverter conv;

RH_NRF24 radio(CONFIG_CE_PIN, CONFIG_CSN_PIN);
RHReliableDatagram radioManager(radio, 0);

void Base::setup() {
    Serial.begin(115200);

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

void Base::loop() {
    /* Wait until something happens on UART or radio */
    __asm__ volatile ("sleep");

    /* We've received and parsed a new JSON message */
    if (conv.obj) {
        Message *msg = MessageJsonConverter::jsonToMessage(*conv.obj);
        aJson.deleteItem(conv.obj);
        conv.obj = NULL;

        /* Succesfully converted to Message */
        if (msg) {
            radioManager.sendtoWait((uint8_t *) msg->getRawData(),
                    msg->getRawLength(), msg->getDstAddress());
            delete msg;
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
                char *str = aJson.print(obj);
                Serial.write(str);
                free(str);
            }
        }
    }
}

ISR(USART_RX_vect) {
    uint8_t status = UCSR0A;
    uint8_t ch = UDR0;

    if (unlikely(status & 0x14))
        return;

    conv.putch(ch);
}

ISR(PCINT0_vect) {
}
ISR(PCINT1_vect) {
}
ISR(PCINT2_vect) {
}

/* vim: set sw=4 ts=4 et: */
