#include <stddef.h>

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

static int messageAddElem(Message *msg, const char *name, aJsonObject *obj) {
    DataType t = Message::stringToDataType(name);
    if (t == (DataType) __INT_MAX__)
        return -1;

    CodingType coding;
    Message::dataTypeToString(t, &coding);

    switch (coding) {
    case boolCoding:
        if (obj->type != aJson_True && obj->type != aJson_False)
            return -1;

        msg->addBoolValue(t, obj->type == aJson_True);
        break;

    case intCoding:
        if (obj->type != aJson_Int)
            return -1;

        msg->addIntValue(t, obj->valueint);
        break;

    case floatCoding:
        if (obj->type != aJson_Int && obj->type != aJson_Float)
            return -1;

        msg->addFloatValue(t, obj->type == aJson_Int ?
                obj->valueint : obj->valuefloat);
        break;

    default:
        return -1;
    }

    return 0;
}

Message *MessageJsonConverter::jsonToMessage(aJsonObject &obj) {
    Message *msg;
    aJsonObject *val;

    if (obj.type != aJson_Object)
        return NULL;

    val = aJson.getObjectItem(&obj, "from");
    if (!val || val->type != aJson_Int)
        return NULL;

    uint8_t from = 0;
    uint8_t to = val->valueint;

    msg = new Message(from, to);

    for (val = obj.child; val; val = val->next) {
        if (!strcasecmp(val->name, "from")) {
            /* Skip */
            continue;
        }

        if (!strcasecmp(val->name, "type")) {
            if (val->type != aJson_String)
                goto err;

            if (!strcmp(val->valuestring, "publish"))
                msg->setType(PUBLISH);
            else if (!strcmp(val->valuestring, "set"))
                msg->setType(SET);
            else if (!strcmp(val->valuestring, "request"))
                msg->setType(REQUEST);
            else if (!strcmp(val->valuestring, "err"))
                msg->setType(ERR);
            else
                goto err;

            continue;
        }

        if (val->type == aJson_Array) {
            for (aJsonObject *elem; elem; elem = elem->next)
                if (messageAddElem(msg, val->name, elem))
                    goto err;
        } else
            if (messageAddElem(msg, val->name, val))
                goto err;
    }

    return msg;

err:
    delete msg;
    return NULL;
}

MessageJsonConverter::MessageJsonConverter() {
    obj_str_len = 0;
    nest_depth = 0;
    quote = 0;
    escape = 0;
}

void MessageJsonConverter::putch(uint8_t chr) {
    if (!quote)
        if (chr <= ' ')
            return;

    if (obj_str_len >= sizeof(obj_str) - 1)
        return;

    obj_str[obj_str_len++] = chr;
    /* TODO: multibyte chars */
    if (quote && !escape && chr == '\\')
        escape = 1;
    else if (!escape && chr == '"')
        quote = !quote;
    else if (!quote) {
        if (chr == '{' || chr == '[' || chr == '(')
            nest_depth++;
        if (chr == '}' || chr == ']' || chr == ')') {
            nest_depth--;

            if (nest_depth <= 0) {
                /* End of JSON object detected */
                obj_str[obj_str_len++] = 0;
                if (!obj)
                    obj = aJson.parse((char *) obj_str);

                obj_str_len = 0;
                nest_depth = 0;
            }
        }
    }
}
/* vim: set sw=4 ts=4 et: */
