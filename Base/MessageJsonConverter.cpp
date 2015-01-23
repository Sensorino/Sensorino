#include <stddef.h>
#include <avr/pgmspace.h>

#include "MessageJsonConverter.h"
#include "Expression.h"

using namespace Data;

static char lower(char chr) {
    if (chr >= 'A' && chr <= 'Z')
        return chr - 'A' + 'a';
}

static bool isEnumType(Type t) {
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
    Message::Type type = m.getType();
    char *typestr = NULL;

    switch (type) {
    case Message::PUBLISH:
        typestr = "publish";
        break;
    case Message::SET:
        typestr = "set";
        break;
    case Message::REQUEST:
        typestr = "request";
        break;
    case Message::ERR:
        typestr = "err";
        break;
    case Message::GARBAGE:
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
    if (to != 0)
        aJson.addNumberToObject(obj, "to", to);
}

void MessageJsonConverter::payloadToJson(aJsonObject *obj, Message &m) {
    for (Message::iter i = m.begin(); i; m.iterAdvance(i)) {
        Type t;
        uint32_t val;
        const prog_char *cname;
        char name[50];
        Message::CodingType coding = (Message::CodingType) -1;
        aJsonObject *parent, *child;
        const prog_char *enumId = NULL;

        m.iterGetTypeValue(i, &t, &val);
        if (t == (Type) -1)
            continue; /* TODO: Add a note in the JSON output */

        cname = Message::dataTypeToString(t, &coding) ?: PSTR("Unknown");
        strncpy_P(name, cname, sizeof(name));
        name[0] = lower(name[0]);

        switch (coding) {
        case Message::boolCoding:
            child = aJson.createItem((char) *(bool *) &val);
            break;
        case Message::intCoding:
            if (isEnumType(t))
                switch (t) {
                case DATATYPE:
                    enumId = Message::dataTypeToString(
                            (Type) *(int *) &val, NULL);
                    break;
                }

            if (enumId) {
                char buf[50];
                strncpy_P(buf, enumId, sizeof(buf));
                child = aJson.createItem(buf);
            } else
                child = aJson.createItem(*(int *) &val);
            break;
        case Message::floatCoding:
            child = aJson.createItem((double) *(float *) &val);
            break;
        case Message::binaryCoding:
            switch (t) {
            case MESSAGE:
                child = aJson.createObject();
                {
                    Message subMsg;
                    memcpy(subMsg.getWriteBuffer() + HEADERS_LENGTH,
                            ((Message::BinaryValue *) &val)->value,
                            ((Message::BinaryValue *) &val)->len);
                    subMsg.writeLength(HEADERS_LENGTH +
                            ((Message::BinaryValue *) &val)->len);
                    payloadToJson(child, subMsg);
                }
                break;

            case EXPRESSION:
                {
                    child = aJson.createNull();
                    child->type = aJson_String;
                    child->valuestring = exprToString(
                            ((Message::BinaryValue *) &val)->value,
                            ((Message::BinaryValue *) &val)->len);
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

static int messageAddElem(Message &msg, const char *name, aJsonObject *obj) {
    Type t = Message::stringToDataType(name);
    if (t == (Type) __INT_MAX__)
        return -1;

    Message::CodingType coding = (Message::CodingType) -1;
    if (!Message::dataTypeToString(t, &coding))
        return -1;

    switch (coding) {
    case Message::boolCoding:
        if (obj->type != aJson_True && obj->type != aJson_False)
            return -1;

        msg.addBoolValue(t, obj->type == aJson_True);
        break;

    case Message::intCoding:
        if (isEnumType(t)) {
            Type valuetype;

            switch (t) {
            case DATATYPE:
                if (obj->type == aJson_String)
                    valuetype = Message::stringToDataType(obj->valuestring);
                else if (obj->type == aJson_Int)
                    valuetype = (Type) obj->valueint;
                else
                    return -1;
                if (valuetype == (Type) __INT_MAX__)
                    return -1;

                msg.addDataTypeValue(valuetype);
                break;
            }
        } else if (obj->type == aJson_Int)
            msg.addIntValue(t, obj->valueint);
        else
            return -1;

        break;

    case Message::floatCoding:
        if (obj->type != aJson_Int && obj->type != aJson_Float)
            return -1;

        msg.addFloatValue(t, obj->type == aJson_Int ?
                obj->valueint : obj->valuefloat);
        break;

    case Message::binaryCoding:
        switch (t) {
        case MESSAGE:
            if (obj->type != aJson_Object)
                return -1;
            {
                Message subMsg(0, 0);
                if (!MessageJsonConverter::jsonToPayload(subMsg, *obj))
                    return -1;
                msg.addBinaryValue(t,
                        subMsg.getRawData() + HEADERS_LENGTH,
                        subMsg.getRawLength() - HEADERS_LENGTH);
            }
            break;

        case EXPRESSION:
            if (obj->type != aJson_String)
                return -1;
            {
                uint8_t len;
                uint8_t *expr = MessageJsonConverter::exprFromString(
                        obj->valuestring, &len);
                if (!expr)
                    return -1;
                msg.addBinaryValue(t, expr, len);
                free(expr);
            }
            break;

        default:
            return -1;
        }
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

    val = aJson.getObjectItem(&obj, "type");
    if (!val || val->type != aJson_String)
        goto err;

    if (!strcmp(val->valuestring, "publish"))
        msg->setType(Message::PUBLISH);
    else if (!strcmp(val->valuestring, "set"))
        msg->setType(Message::SET);
    else if (!strcmp(val->valuestring, "request"))
        msg->setType(Message::REQUEST);
    else if (!strcmp(val->valuestring, "err"))
        msg->setType(Message::ERR);
    else
        goto err;

    if (!jsonToPayload(*msg, obj))
        goto err;

    return msg;

err:
    delete msg;
    return NULL;
}

bool MessageJsonConverter::jsonToPayload(Message &msg, aJsonObject &obj) {
    aJsonObject *val;

    for (val = obj.child; val; val = val->next) {
        if (!strcasecmp(val->name, "to") || !strcasecmp(val->name, "from") ||
                !strcasecmp(val->name, "type")) {
            /* Skip */
            continue;
        }

        if (val->type == aJson_Array) {
            for (aJsonObject *elem = val->child; elem; elem = elem->next)
                if (messageAddElem(msg, val->name, elem))
                    return 0;
        } else
            if (messageAddElem(msg, val->name, val))
                return 0;
    }

    return 1;
}

/* TODO: pgmspace */
struct {
    uint8_t val;
    char str[2];
} opTable[] PROGMEM = {
    { Expression::OP_EQ, '=', '=' },
    { Expression::OP_NE, '!', '=' },
    { Expression::OP_LT, "<" },
    { Expression::OP_GT, ">" },
    { Expression::OP_LE, '<', '=' },
    { Expression::OP_GE, '>', '=' },
    { Expression::OP_OR, '|', '|' },
    { Expression::OP_AND, '&', '&' },
    { Expression::OP_ADD, "+" },
    { Expression::OP_SUB, "-" },
    { Expression::OP_MULT, "*" },
    { Expression::OP_DIV, "/" },
    { Expression::OP_IN, 'i', 'n' },
    { Expression::OP_IFELSE, "?" },
    { 0, 0 },
};

static void numDigit(char *&str, int16_t val) {
    if (!val)
        return;
    numDigit(str, val / 10);
    *str++ = '0' + (val % 10);
}

static float numDigit(char *&str, float val) {
    if (val >= 10.0f)
        val = numDigit(str, val / 10.0f) * 10.0f;
    *str++ = '0' + (int) val;
    return val - (int) val;
}

static void numToStr(char *&str, int16_t val) {
    if (val == 0)
        *str++ = '0';
    else {
        if (val < 0) {
            *str++ = '-';
            val = -val;
        }
        numDigit(str, val);
    }
}

static void numToStr(char *&str, float val) {
    uint16_t frac;

    if (val < 0.0f) {
        *str++ = '-';
        val = -val;
    }
    frac = numDigit(str, val) * 10000.0f;
    if (frac) {
        *str++ = '.';
    }
    while (frac) {
        uint8_t dig = frac / 1000;
        frac = (frac % 1000) * 10;
        *str++ = '0' + dig;
    }
}

void subexprToString(char *&str, const uint8_t *&buf, uint8_t &len) {
    int intVal;
    float floatVal;
    uint8_t varServId, varType, varNum, num;
    const char *name;

    if (!len) {
        memcpy_P(str, PSTR("fixme"), 5);
        str += 5;
        return;
    }

    len--;
    uint8_t op = *buf++;

    using namespace Expression;

    if (op >= OP_EQ)
        *str++ = '(';

#define SPACE *str++ = ' '

    switch (op) {
    case VAL_INT8:
        intVal = (int8_t) *buf++;
        len--;
        numToStr(str, intVal);
        break;

    case VAL_INT16:
        intVal = ((uint16_t) buf[0] << 8) | buf[1];
        buf += 2;
        len -= 2;
        numToStr(str, intVal);
        break;

    case VAL_FLOAT:
        *(uint32_t *) &floatVal =
            ((uint32_t) buf[0] << 24) |
            ((uint32_t) buf[1] << 16) |
            ((uint32_t) buf[2] <<  8) |
            ((uint32_t) buf[3] <<  0);
        buf += 4;
        len -= 4;
        numToStr(str, floatVal);
        break;

    case VAL_VARIABLE:
    case VAL_PREVIOUS:
        varServId = *buf++;
        varType = (Type) *buf++;
        varNum = *buf++;
        len -= 3;

        memcpy_P(str, op == VAL_VARIABLE ? PSTR("data:") : PSTR("prev:"), 5);
        str += 5;

        numToStr(str, varServId);
        *str++ = ':';
        name = Message::dataTypeToString((Type) varType, NULL) ?: PSTR("fixme");
        strcpy_P(str, name);
        str += strlen_P(name);
        *str++ = ':';
        numToStr(str, varNum);
        break;

    case OP_EQ:
    case OP_NE:
    case OP_LT:
    case OP_GT:
    case OP_LE:
    case OP_GE:
    case OP_OR:
    case OP_AND:
    case OP_ADD:
    case OP_SUB:
    case OP_MULT:
    case OP_DIV:
        subexprToString(str, buf, len);
        for (num = 0; pgm_read_byte(&opTable[num].val) != op; num++);
        SPACE;
        *str++ = pgm_read_byte(&opTable[num].str[0]);
        if (pgm_read_byte(&opTable[num].str[1]))
            *str++ = pgm_read_byte(&opTable[num].str[1]);
        SPACE;
        subexprToString(str, buf, len);
        break;

    case OP_NOT:
    case OP_NEG:
        *str++ = (op == OP_NOT) ? '!' : '-';
        subexprToString(str, buf, len);
        break;

    case OP_IN:
        num = *buf++;
        len--;
        subexprToString(str, buf, len);
        memcpy_P(str, PSTR(" in "), 4);
        str += 4;

        while (--num) {
            subexprToString(str, buf, len);
            *str++ = ',';
        }
        subexprToString(str, buf, len);
        break;

    case OP_IFELSE:
        subexprToString(str, buf, len);
        SPACE;
        *str++ = '?';
        SPACE;
        subexprToString(str, buf, len);
        SPACE;
        *str++ = ':';
        SPACE;
        subexprToString(str, buf, len);
        break;

    case OP_BETWEEN:
        subexprToString(str, buf, len);
        memcpy_P(str, PSTR(" between "), 9);
        str += 9;
        subexprToString(str, buf, len);
        *str++ = ',';
        subexprToString(str, buf, len);
    }

    if (op >= OP_EQ)
        *str++ = ')';
}

char *MessageJsonConverter::exprToString(const uint8_t *buf, uint8_t len) {
    char *str = (char *) malloc(128); /* FIXME */
    char *ptr = str;

    subexprToString(ptr, buf, len);
    *ptr = '\0';

    return str;
}

#define isDigit(x) ((x) >= '0' && (x) <= '9')

static uint8_t uint8FromString(const char *&str) {
    uint8_t val;
    while (isDigit(*str))
        val = val * 10 + (*str++ - '0');
    return val;
}

static void valueFromString(uint8_t *&buf, const char *&str) {
    if (!memcmp_P(str, PSTR("data:"), 5) || !memcmp_P(str, PSTR("prev:"), 5)) {
        char name[30], len = 0;

        /* Opcode byte */
        *buf++ = (*str == 'd') ?
            Expression::VAL_VARIABLE : Expression::VAL_PREVIOUS;
        str += 5;

        /* Service ID byte */
        *buf++ = uint8FromString(str);

        if (*str++ != ':')
            return;

        /* Data::Type byte */
        while (*str != ':' && *str != '\0' && len < sizeof(name) - 1)
            name[len++] = *str++;
        name[len] = '\0';
        *buf = (int) Message::stringToDataType(name);
        if (*buf++ == 0xff)
            return;

        if (*str++ != ':')
            return;

        /* Data position byte */
        *buf++ = uint8FromString(str);
        return;
    }

    float val = 0;
    bool negative = 0;

    if (*str == '-') {
        str++;
        negative = 1;
    }
    while (isDigit(*str))
        val = val * 10 + (*str++ - '0');
    if (*str != '.') {
        int16_t intVal = negative ? -val : val;

        /* See if number is within integer range */
        if (val <= 127.5f + negative) {
            *buf++ = Expression::VAL_INT8;
            *buf++ = intVal;
            return;
        }
        if (val <= 32767.5f + negative) {
            *buf++ = Expression::VAL_INT16;
            *buf++ = intVal >> 8;
            *buf++ = intVal;
            return;
        }
        /* Nope, encode it as a float anyway */
    } else {
        float fracUnit = 0.1f;

        str++;
        while (isDigit(*str)) {
            val += fracUnit * (*str++ - '0');
            fracUnit *= 0.1f;
        }
    }
    if (negative)
        val = -val;
    *buf++ = Expression::VAL_FLOAT;
    *buf++ = (*(uint32_t *) &val) >> 24;
    *buf++ = (*(uint32_t *) &val) >> 16;
    *buf++ = (*(uint32_t *) &val) >>  8;
    *buf++ = (*(uint32_t *) &val) >>  0;
}

static bool subexprFromString(uint8_t *&buf, const char *&str) {
    uint8_t *in_counter = NULL;
    bool ifelse = 0, between = 0;

    while (1) {
        uint8_t space, op, num, *start = buf;

        using namespace Expression;

        if (*str == '(') {
            str++;
            if (!subexprFromString(buf, str) || *str != ')')
                return 0;
            str++;
        } else if (*str == '!' || (*str == '-' && !isDigit(str[1]))) {
            *buf ++ = (*str++ == '!') ? OP_NOT : OP_NEG;
            if (!subexprFromString(buf, str))
                return 0;
        } else
            valueFromString(buf, str);

        while (*str == ' ')
            str++;

        if (*str == ')' || *str == '\0')
            break;

        for (num = 0; pgm_read_byte(&opTable[num].val); num++)
            if (str[0] == pgm_read_byte(&opTable[num].str[0]) &&
                    (str[1] == pgm_read_byte(&opTable[num].str[1]) ||
                     !pgm_read_byte(&opTable[num].str[1])))
                break;
        op = pgm_read_byte(&opTable[num].val);
        space = 1;
        if (op) {
            str++;
            if (pgm_read_byte(&opTable[num].str[1]))
                str++;
            if (op == OP_IN)
                space = 2;
        } else if (!memcmp_P(str, PSTR("between"), 7)) {
            op = OP_BETWEEN;
            str += 7;
            between = 1;
        } else if (str[0] == ',') {
            if (in_counter)
                *in_counter++;
            else if (between)
                between = 0;
            else
                break;
        } else if (str[0] == ':') {
            if (ifelse)
                ifelse = 0;
            else
                break;
        } else
            return 0;

        while (*str == ' ')
            str++;

        if (!op)
            continue;
        if (op == OP_IN)
            space = 2;

        for (uint8_t *ptr = buf - 1; ptr >= start; ptr--)
            ptr[space] = ptr[0];
        buf += space;
        start[0] = op;

        if (op == OP_IN) {
            in_counter = start + 1;
            *in_counter = 1;
        } else if (op == OP_IFELSE)
            ifelse = 1;
    }

    if (between || ifelse)
        return 0;

    return 1;
}

uint8_t *MessageJsonConverter::exprFromString(const char *str, uint8_t *len) {
    uint8_t *buf = (uint8_t *) malloc(128); /* FIXME */
    uint8_t *ptr = buf;

    if (!subexprFromString(ptr, str) || *str != '\0') {
        free(buf);
        return NULL;
    }

    *len = ptr - buf;
    return buf;
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
