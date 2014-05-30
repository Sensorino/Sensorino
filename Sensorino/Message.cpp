#include <string.h>

#include "Message.h"
#include "../Sensorino/Sensorino.h"

// https://en.wikipedia.org/wiki/Basic_Encoding_Rules#BER_encoding
const uint8_t extendedType=0b00011111;
const uint8_t booleanType=1;
const uint8_t intType=2;
const uint8_t nullType=5;
const uint8_t floatType=9;
const uint8_t charStringType=29;

/* TODO: move to progmem */
static const struct TypeInfo {
    DataType val;
    const char *name;
    CodingType coding;
} typeTable[] = {
#define TYPEINFO_INITIALISER(intval, CAPS, Camel, coding) \
    { CAPS, #Camel, glue(coding, Coding) },
DATATYPE_LIST_APPLY(TYPEINFO_INITIALISER)
    { (DataType) __INT_MAX__, NULL, (enum CodingType) -1 }, /* Sentinel */
};

static const struct TypeInfo *getTypeInfo(DataType type) {
    const struct TypeInfo *i = typeTable;

    while (type > i->val)
        i++;

    return type == i->val ? i : NULL;
}

const char *Message::dataTypeToString(DataType type, CodingType *coding) {
    const struct TypeInfo *i = getTypeInfo(type);

    if (coding && i)
        *coding = i->coding;

    return i ? i->name : NULL;
}

DataType Message::stringToDataType(const char *str) {
    const struct TypeInfo *i = typeTable;

    while (i->val < __INT_MAX__ && strcasecmp(str, i->name))
        i++;

    return i->val;
}

uint8_t Message::staticId;

Message::Message(uint8_t src, uint8_t dst) {
    staticId++;
    if (staticId >= MAX_MESSAGE_ID)
        staticId -= MAX_MESSAGE_ID;
    setId(staticId);

    setSrcAddress(src);
    setDstAddress(dst);

    rawLen = HEADERS_LENGTH;
}

Message::Message(const uint8_t *raw, int len) {
    if (len > HEADERS_LENGTH + PAYLOAD_LENGTH || len < HEADERS_LENGTH) {
        /* Can't Sensorino::die here because this may be a network error */
        rawLen = HEADERS_LENGTH;
        setType(GARBAGE);
        return;
    }

    rawLen = len;
    for (len = 0; len < rawLen; len++)
        Message::raw[len] = raw[len];
}

bool Message::send(void) {
    bool ret = sensorino->sendMessage(*this);

    /* This is a bit of a hack, we can probably do this in a callback set
     * by whoever allocated this message (e.g. Service::startBaseMessage) */
    delete this;

    return ret;
}

uint8_t Message::getId(){
    return raw[3];
}

void Message::setId(uint8_t i){
    raw[3] = i;
}

uint8_t Message::getSrcAddress() {
    return raw[0];
}

void Message::setSrcAddress(uint8_t addr) {
    raw[0] = addr;
}

uint8_t Message::getDstAddress() {
    return raw[1];
}

void Message::setDstAddress(uint8_t addr) {
    raw[1] = addr;
}

MessageType Message::getType(void){
    return (MessageType) raw[2];
}

void Message::setType(MessageType t){
    raw[2] = t;
}

const uint8_t *Message::getRawData(void) {
    return raw;
}


int Message::getRawLength(void) {
    return rawLen;
}

/* For now use the handcrafted macros to avoid dependency on the big table */
#define BOOL_TYPE(t) (t == PRESENCE || t == SWITCH)
#define INT_TYPE(t) (t == DATATYPE || t == COUNT || t == SERVICE_ID)
#define FLOAT_TYPE(t) (t >= ACCELERATION && t < COUNT)

int Message::find(DataType t, int num, void *value) {
    int pos = HEADERS_LENGTH, len;

    while (pos < rawLen - 3) {
        if (raw[pos++] != extendedType || raw[pos++] != (int) t || num--) {
            /* Skip this TLV */
            len = raw[pos++];
            pos += len;
            continue;
        }

        len = raw[pos++];
#define CHECK_LENGTH(n) (pos + n > HEADERS_LENGTH + PAYLOAD_LENGTH || \
        len < n)

        /* Is this type serialised as a boolean, int or float? */
        if (BOOL_TYPE(t)) {
            int bool_val;

            if (CHECK_LENGTH(1))
                return 0;

            bool_val = raw[pos] != 0;
            *(int *) value = bool_val;
        } else if (FLOAT_TYPE(t)) {
            uint32_t float_val;

            if (CHECK_LENGTH(4))
                return 0;

            float_val = raw[pos++];
            float_val |= raw[pos++] << 8;
            float_val |= raw[pos++] << 16;
            float_val |= raw[pos++] << 24;
            *(uint32_t *) value = float_val;
        } else if (INT_TYPE(t)) {
            uint16_t int_val;

            if (CHECK_LENGTH(2))
                return 0;

            int_val = raw[pos++] << 8;
            int_val |= raw[pos];
            *(int *) value = (int16_t) int_val;
        }

        return 1;
    }

    return 0;
}

int _addInt(uint8_t *buffer, int value){
    // Value
    for (int i=0;i<sizeof(int);i++){
        buffer[sizeof(int)-1-i]=(value>>i*8)&0xFF;
    }
    return sizeof(int);
}

void Message::addIntValue(DataType t, int value){
    // Type
    raw[rawLen++]=extendedType; // extended type
    raw[rawLen++]=t;

    // Len + Value
    int length=_addInt(raw+rawLen+1, value);
    raw[rawLen]=length;
    rawLen+=1+length;

    checkIntegrity();
}


inline void Message::checkIntegrity(){
    if (rawLen> HEADERS_LENGTH + PAYLOAD_LENGTH){
        //We should stop adding stuff or we'll crash
        Sensorino::die("constructing message bigger that max size");
    }
}



void Message::addInt(int value){
    // Type
    raw[rawLen++]=intType;
    // Len + Value
    int length=_addInt(raw+rawLen+1, value);
    raw[rawLen]=length;
    rawLen+=1+length;
}

void Message::addFloatValue(DataType t, float value){
    // Type
    raw[rawLen++]=extendedType; // extended type
    raw[rawLen++]=t;

    raw[rawLen++]=4;

    unsigned long d = *(unsigned long *)&value;
    raw[rawLen++]=d & 0x00FF;
    raw[rawLen++]=(d & 0xFF00) >> 8;
    raw[rawLen++]=(d & 0xFF0000) >> 16;
    raw[rawLen++]=(d & 0xFF000000) >> 24;


    checkIntegrity();
}

void Message::addDataTypeValue(DataType t){
    addIntValue(DATATYPE, t);
}

void Message::addBoolValue(DataType t, int value){
    // Type
    raw[rawLen++]=extendedType; // extended type
    raw[rawLen++]=t;

    // Len + Value
    raw[rawLen++]=1;
    raw[rawLen++]=!!value;

    checkIntegrity();
}

#define int(...)
#define bool(...)
#define float(CAPS_NAME, CamelName) \
\
void Message::glue(add, CamelName)(float val) { \
    addFloatValue(CAPS_NAME, val); \
} \
\
void Message::glue(add, CamelName)(int val) { \
    addFloatValue(CAPS_NAME, val); \
}
#define FLOAT_INT_ACCESSOR_IMPL(intval, CAPS, Camel, coding) coding(CAPS, Camel)
DATATYPE_LIST_APPLY(FLOAT_INT_ACCESSOR_IMPL)
#undef float
#undef bool
#undef int

Message::iter Message::begin() {
    return rawLen > HEADERS_LENGTH ? HEADERS_LENGTH : 0;
}

void Message::iterAdvance(Message::iter &i) {
    if (i > rawLen - 3)
        i = 0;
    else {
        if (raw[i++] == extendedType)
           i++;
        int len = raw[i++];
        i += len;
        if (i >= rawLen)
            i = 0;
    }
}

void Message::iterGetTypeValue(Message::iter i, DataType *type, void *val) {
    DataType t;

    /* Read type */
    if (raw[i++] != extendedType) {
        *type = (DataType) -1;
        return;
    }

    t = (DataType) raw[i++];
    if (type)
        *type = t;

    /* Skip length.  TODO: multiple values in a TLV */
    i++;

    /* Read value */
    if (val) {
        if (BOOL_TYPE(t))
            *(int *) val = raw[i] != 0;
        else if (FLOAT_TYPE(t)) {
            uint32_t float_val;

            float_val = raw[i++];
            float_val |= raw[i++] << 8;
            float_val |= raw[i++] << 16;
            float_val |= raw[i++] << 24;
            *(uint32_t *) val = float_val;
        } else if (INT_TYPE(t)) {
            uint16_t int_val;

            int_val = raw[i++] << 8;
            int_val |= raw[i];

            *(int *) val = int_val;
        }
    }
}

/* vim: set sw=4 ts=4 et: */
