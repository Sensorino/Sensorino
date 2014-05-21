#include <stddef.h>

#include "Message.h"
#include "MessageJsonConverter.h"

aJsonObject *MessageJsonConverter::messageToJson(Message &m) {
    aJsonObject *obj = aJson.createObject();

    /* Convert the header first */
    uint8_t from = m.getSrcAddress();
    uint8_t to = m.getDstAddress();
    MessageType type = m.getType();
    char *typestr = NULL;

    switch (type) {
    case PUBLISH:
        typestr = "publish";
        break;
    case SET:
        typestr = "set";
        break;
    case REQUEST:
        typestr = "request";
        break;
    case ERR:
        typestr = "err";
        break;
    case GARBAGE:
        typestr = "garbage";
        break;
    default:
        typestr = "unknown";
    }

    if (typestr)
        aJson.addStringToObject(obj, "type", typestr);
    else
        aJson.addNumberToObject(obj, "type", type);
    aJson.addNumberToObject(obj, "from", from);

    for (Message::iter i = m.begin(); i; m.iterAdvance(i)) {
        DataType t;
        uint32_t val;
        const char *name;
        CodingType coding;

        m.iterGetTypeValue(i, &t, &val);
        if (t == (DataType) -1)
            continue; /* TODO: Add a note in the JSON output */

        name = Message::dataTypeToString(t, &coding);

        /* TODO: check if name is a duplicate.. if so, create a list.
         * This is required because a JSON object is an unordered set
         * of name/value pairs, so the order is lost if we just add
         * a second pair with the same name.
         */
        switch (coding) {
        case boolCoding:
            aJson.addBooleanToObject(obj, name, (bool) *(int *) val);
            break;
        case intCoding:
            aJson.addNumberToObject(obj, name, *(int *) val);
            break;
        case floatCoding:
            aJson.addNumberToObject(obj, name, (double) *(float *) val);
            break;
        default:
            aJson.addStringToObject(obj, name, "fixme");
            break;
        }
    }

    return obj;
}
/* vim: set sw=4 ts=4 et: */
