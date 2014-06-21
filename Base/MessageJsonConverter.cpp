#include <stddef.h>

#include "MessageJsonConverter.h"

static char lower(char chr) {
    if (chr >= 'A' && chr <= 'Z')
        return chr - 'A' + 'a';
}

static bool isEnumType(DataType t) {
    return t == DATATYPE;
}

aJsonObject *MessageJsonConverter::messageToJson(Message &m) {
    aJsonObject *obj = aJson.createObject();

    headerToJson(obj, m);
    payloadToJson(obj, m);

    return obj;
}

void MessageJsonConverter::headerToJson(aJsonObject *obj, Message &m) {
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
}

void MessageJsonConverter::payloadToJson(aJsonObject *obj, Message &m) {
    for (Message::iter i = m.begin(); i; m.iterAdvance(i)) {
        DataType t;
        uint32_t val;
        const prog_char *cname;
        char name[50];
        CodingType coding = (CodingType) -1;
        aJsonObject *parent, *child;
        const prog_char *enumId = NULL;

        m.iterGetTypeValue(i, &t, &val);
        if (t == (DataType) -1)
            continue; /* TODO: Add a note in the JSON output */

        cname = Message::dataTypeToString(t, &coding) ?: PSTR("Unknown");
        strncpy_P(name, cname, sizeof(name));
        name[0] = lower(name[0]);

        switch (coding) {
        case boolCoding:
            child = aJson.createItem((char) *(bool *) &val);
            break;
        case intCoding:
            if (isEnumType(t))
                switch (t) {
                case DATATYPE:
                    enumId = Message::dataTypeToString(
                            (DataType) *(int *) &val, NULL);
                    break;
                }

            if (enumId) {
                char buf[50];
                strncpy_P(buf, enumId, sizeof(buf));
                child = aJson.createItem(buf);
            } else
                child = aJson.createItem(*(int *) &val);
            break;
        case floatCoding:
            child = aJson.createItem((double) *(float *) &val);
            break;
        case binaryCoding:
            switch (t) {
            case MESSAGE:
                child = aJson.createObject();
                {
                    Message subMsg;
                    memcpy(subMsg.getWriteBuffer() + HEADERS_LENGTH,
                            ((BinaryValue *) &val)->value,
                            ((BinaryValue *) &val)->len);
                    subMsg.writeLength(HEADERS_LENGTH +
                            ((BinaryValue *) &val)->len);
                    payloadToJson(child, subMsg);
                }
                break;
            default:
                child = aJson.createItem("fixme");
            }
            break;
        default:
            child = aJson.createItem("fixme");
            break;
        }

        /* See if there is an item of the same name already. */
        parent = aJson.getObjectItem(obj, name); /* slow lookup :/ */

        /* If there is one and it's not an array, convert it into a
         * one-element array so we can append the new item to it.  This
         * is necessary because, even though JSON objects don't
         * explicitly disallow pairs with identical names, an object
         * is defined as being unordered meaning that the order of
         * same-typed elements would be lost, unlike in a list.
         */
        if (parent) {
            if (parent->type != aJson_Array) {
                /* Low-level but less inefficient */
                aJsonObject *copy = aJson.createNull();
                copy->type = parent->type;
                copy->child = parent->child;
                copy->valuefloat = parent->valuefloat;
                parent->type = aJson_Array;
                parent->child = NULL;
                aJson.addItemToArray(parent, copy);
            }
            aJson.addItemToArray(parent, child);
        } else
            aJson.addItemToObject(obj, name, child);
    }
}

static int messageAddElem(Message *msg, const char *name, aJsonObject *obj) {
    DataType t = Message::stringToDataType(name);
    if (t == (DataType) __INT_MAX__)
        return -1;

    CodingType coding = (CodingType) -1;
    if (!Message::dataTypeToString(t, &coding))
        return -1;

    switch (coding) {
    case boolCoding:
        if (obj->type != aJson_True && obj->type != aJson_False)
            return -1;

        msg->addBoolValue(t, obj->type == aJson_True);
        break;

    case intCoding:
        if (isEnumType(t)) {
            DataType valuetype;

            switch (t) {
            case DATATYPE:
                if (obj->type == aJson_String)
                    valuetype = Message::stringToDataType(obj->valuestring);
                else if (obj->type == aJson_Int)
                    valuetype = (DataType) obj->valueint;
                else
                    return -1;
                if (valuetype == (DataType) __INT_MAX__)
                    return -1;

                msg->addDataTypeValue(valuetype);
                break;
            }
        } else if (obj->type == aJson_Int)
            msg->addIntValue(t, obj->valueint);
        else
            return -1;

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
    uint8_t from, to;

    if (obj.type != aJson_Object)
        return NULL;

    val = aJson.getObjectItem(&obj, "to");
    if (!val || val->type != aJson_Int)
        return NULL;
    to = val->valueint;

    val = aJson.getObjectItem(&obj, "from");
    if (val && val->type != aJson_Int)
        return NULL;
    from = val ? val->valueint : 0;

    msg = new Message(from, to);

    for (val = obj.child; val; val = val->next) {
        if (!strcasecmp(val->name, "to") || !strcasecmp(val->name, "from")) {
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
            for (aJsonObject *elem = val->child; elem; elem = elem->next)
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

            if ((int8_t) nest_depth <= 0) {
                /* End of JSON object detected */
                obj_str[obj_str_len++] = 0;
                if (!obj) {
                    obj = aJson.parse((char *) obj_str);
                    if (!obj)
                        obj = aJson.parse("{\"error\":\"syntaxError\"}");
                }

                obj_str_len = 0;
                nest_depth = 0;
            }
        }
    }
}
/* vim: set sw=4 ts=4 et: */
