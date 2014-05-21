#ifndef MESSAGE_JSON_CONVERTER_H
#define MESSAGE_JSON_CONVERTER_H

#include <aJSON.h>

#include "Message.h"

class MessageJsonConverter {
public:
    /* Json -> Message conversion (stateful) */
    MessageJsonConverter();
    void putch(uint8_t chr);
    aJsonObject *obj;

    /* Message -> Json conversion (stateless) */
    static aJsonObject *messageToJson(Message &m);

private:
    uint8_t obj_str[128];
};

#endif // whole file
/* vim: set sw=4 ts=4 et: */
