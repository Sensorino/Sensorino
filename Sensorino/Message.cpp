#include <Message.h>

// https://en.wikipedia.org/wiki/Basic_Encoding_Rules#BER_encoding
const uint8_t extendedType=0b00011111;
const uint8_t booleanType=1;
const uint8_t intType=2;
const uint8_t nullType=5;
const uint8_t floatType=9;
const uint8_t charStringType=29;

const char *Message::dataTypeToString(DataType type){
    switch(type)
    {
        case ACCELERATION:
            return "ACCELERATION";
        case AMOUNT:
            return "AMOUNT";
        case ANGLE:
            return "ANGLE";
        case ANGULAR_VELOCITY:
            return "ANGULAR_VELOCITY";
        case AREA:
            return "AREA";
        case RADIOACTIVITY:
            return "RADIOACTIVITY";
        case ELECTRICAL_CAPACITANCE:
            return "ELECTRICAL_CAPACITANCE";
        case ELECTRICAL_RESISTANCE:
            return "ELECTRICAL_RESISTANCE";
        case ELECTRIC_CURRENT:
            return "ELECTRIC_CURRENT";
        case ENERGY:
            return "ENERGY";
        case FORCE:
            return "FORCE";
        case FREQUENCY:
            return "FREQUENCY";
        case ILLUMINANCE:
            return "ILLUMINANCE";
        case INDUCTANCE:
            return "INDUCTANCE";
        case LENGTH:
            return "LENGTH";
        case LUMINOUS_FLUX:
            return "LUMINOUS_FLUX";
        case LUMINOUS_INTENSITY:
            return "LUMINOUS_INTENSITY";
        case MAGNETIC_FIELD_STRENGTH:
            return "MAGNETIC_FIELD_STRENGTH";
        case MASS:
            return "MASS";
        case POWER:
            return "POWER";
        case PRESSURE:
            return "PRESSURE";
        case RELATIVE_HUMIDITY:
            return "RELATIVE_HUMIDITY";
        case SPEED:
            return "SPEED";
        case TEMPERATURE:
            return "TEMPERATURE";
        case TIME:
            return "TIME";
        case VOLTAGE:
            return "VOLTAGE";
        case VOLUME:
            return "VOLUME";

        default:
            return "UNKNOWN";
    }
}

Message::Message(uint8_t src, uint8_t dst) {
    staticId++;
    if (staticId >= MAX_MESSAGE_ID)
        staticId -= MAX_MESSAGE_ID;
    setId(staticId);

    setSrcAddress(src);
    setDstAddress(dst);

    rawLen = HEADERS_LENGTH;
}

Message::Message(uint8_t *raw, int len) {
    if (len > HEADERS_LENGTH + PAYLOAD_LENGTH)
        Sensorino::die("Message too big");

    memcpy(Message::raw, raw, len);
    rawLen = len;
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
    return raw[2];
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

#define BOOL_TYPE(t) (t == PRESENCE || t == SWITCH)
#define INT_TYPE(t) (t == DATATYPE || t == COUNT || t == SERVICE_ID)
#define FLOAT_TYPE(t) (t >= ACCELERATION && T < COUNT)

int Message::find(DataType t, int num, void *value) {
    int pos = HEADERS_LENGTH, len;

    while (pos < rawLen - 3) {
        if (raw[pos++] != extendedType || raw[pos++] != (int) t || num--) {
            /* Skip this TLV */
            len = raw[pos++]
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

            bool_val = raw[pos++] != 0;
            *(int *) value = bool_val;
        } else if (FLOAT_TYPE(t)) {
            if (CHECK_LENGTH(4))
                return 0;

            /* Avoid alignment traps, TODO: endianness */
            memcpy(value, raw + pos, 4);
            pos += 4;
        } else if (INT_TYPE(t)) {
            uint16_t int_val;

            if (CHECK_LENGTH(2))
                return 0;

            int_val = raw[pos++] << 8;
            int_val |= raw[pos++];
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
    payload[payloadLen++]=extendedType; // extended type
    payload[payloadLen++]=t;

    // Len + Value
    int length=_addInt(payload+payloadLen+1, value);
    payload[payloadLen]=length;
    payloadLen+=1+length;

}

void Message::addInt(int value){
    // Type
    payload[payloadLen++]=intType;
    // Len + Value
    int length=_addInt(payload+payloadLen+1, value);
    payload[payloadLen]=length;
    payloadLen+=1+length;
}

void Message::addFloatValue(DataType t, float value){
    // Type
    payload[payloadLen++]=extendedType; // extended type
    payload[payloadLen++]=t;

    payload[payloadLen++]=4;

    unsigned long d = *(unsigned long *)&value;
    payload[payloadLen++]=d & 0x00FF;
    payload[payloadLen++]=(d & 0xFF00) >> 8;
    payload[payloadLen++]=(d & 0xFF0000) >> 16;
    payload[payloadLen++]=(d & 0xFF000000) >> 24;
}

void addDataTypeValue(DataType t){
    addIntValue(DATATYPE, t);
}

void addBoolValue(DataType t, int value){
    // Type
    payload[payloadLen++]=extendedType; // extended type
    payload[payloadLen++]=t;

    // Len + Value
    payload[payloadLen++]=1;
    payload[payloadLen++]=!!value;
}

void Message::addTemperature(float temperature){
    addFloatValue(TEMPERATURE, temperature);
}
void Message::addTemperature(int temperature){
    addFloatValue(TEMPERATURE, (float)temperature);
}
void Message::addAcceleration(float acceleration){
    addFloatValue(ACCELERATION, acceleration);
}
void Message::addAcceleration(int acceleration){
    addFloatValue(ACCELERATION, (float)acceleration);
}
void Message::addAmount(float amount){
    addFloatValue(AMOUNT, amount);
}
void Message::addAmount(int amount){
    addFloatValue(AMOUNT, (float)amount);
}
void Message::addAngle(float angle){
    addFloatValue(ANGLE, angle);
}
void Message::addAngle(int angle){
    addFloatValue(ANGLE, (float)angle);
}
void Message::addAngular_velocity(float angular_velocity){
    addFloatValue(ANGULAR_VELOCITY, angular_velocity);
}
void Message::addAngular_velocity(int angular_velocity){
    addFloatValue(ANGULAR_VELOCITY, (float)angular_velocity);
}
void Message::addArea(float area){
    addFloatValue(AREA, area);
}
void Message::addArea(int area){
    addFloatValue(AREA, (float)area);
}
void Message::addRadioactivity(float radioactivity){
    addFloatValue(RADIOACTIVITY, radioactivity);
}
void Message::addRadioactivity(int radioactivity){
    addFloatValue(RADIOACTIVITY, (float)radioactivity);
}
void Message::addElectrical_capacitance(float electrical_capacitance){
    addFloatValue(ELECTRICAL_CAPACITANCE, electrical_capacitance);
}
void Message::addElectrical_capacitance(int electrical_capacitance){
    addFloatValue(ELECTRICAL_CAPACITANCE, (float)electrical_capacitance);
}
void Message::addElectrical_resistance(float electrical_resistance){
    addFloatValue(ELECTRICAL_RESISTANCE, electrical_resistance);
}
void Message::addElectrical_resistance(int electrical_resistance){
    addFloatValue(ELECTRICAL_RESISTANCE, (float)electrical_resistance);
}
void Message::addElectric_current(float electric_current){
    addFloatValue(ELECTRIC_CURRENT, electric_current);
}
void Message::addElectric_current(int electric_current){
    addFloatValue(ELECTRIC_CURRENT, (float)electric_current);
}
void Message::addEnergy(float energy){
    addFloatValue(ENERGY, energy);
}
void Message::addEnergy(int energy){
    addFloatValue(ENERGY, (float)energy);
}
void Message::addForce(float force){
    addFloatValue(FORCE, force);
}
void Message::addForce(int force){
    addFloatValue(FORCE, (float)force);
}
void Message::addFrequency(float frequency){
    addFloatValue(FREQUENCY, frequency);
}
void Message::addFrequency(int frequency){
    addFloatValue(FREQUENCY, (float)frequency);
}
void Message::addIlluminance(float illuminance){
    addFloatValue(ILLUMINANCE, illuminance);
}
void Message::addIlluminance(int illuminance){
    addFloatValue(ILLUMINANCE, (float)illuminance);
}
void Message::addInductance(float inductance){
    addFloatValue(INDUCTANCE, inductance);
}
void Message::addInductance(int inductance){
    addFloatValue(INDUCTANCE, (float)inductance);
}
void Message::addLength(float length){
    addFloatValue(LENGTH, length);
}
void Message::addLength(int length){
    addFloatValue(LENGTH, (float)length);
}
void Message::addLuminous_flux(float luminous_flux){
    addFloatValue(LUMINOUS_FLUX, luminous_flux);
}
void Message::addLuminous_flux(int luminous_flux){
    addFloatValue(LUMINOUS_FLUX, (float)luminous_flux);
}
void Message::addLuminous_intensity(float luminous_intensity){
    addFloatValue(LUMINOUS_INTENSITY, luminous_intensity);
}
void Message::addLuminous_intensity(int luminous_intensity){
    addFloatValue(LUMINOUS_INTENSITY, (float)luminous_intensity);
}
void Message::addMagnetic_field_strength(float magnetic_field_strength){
    addFloatValue(MAGNETIC_FIELD_STRENGTH, magnetic_field_strength);
}
void Message::addMagnetic_field_strength(int magnetic_field_strength){
    addFloatValue(MAGNETIC_FIELD_STRENGTH, (float)magnetic_field_strength);
}
void Message::addMass(float mass){
    addFloatValue(MASS, mass);
}
void Message::addMass(int mass){
    addFloatValue(MASS, (float)mass);
}
void Message::addPower(float power){
    addFloatValue(POWER, power);
}
void Message::addPower(int power){
    addFloatValue(POWER, (float)power);
}
void Message::addPressure(float pressure){
    addFloatValue(PRESSURE, pressure);
}
void Message::addPressure(int pressure){
    addFloatValue(PRESSURE, (float)pressure);
}
void Message::addHumidity(float relative_humidity){
    addFloatValue(RELATIVE_HUMIDITY, relative_humidity);
}
void Message::addHumidity(int relative_humidity){
    addFloatValue(RELATIVE_HUMIDITY, (float)relative_humidity);
}
void Message::addSpeed(float speed){
    addFloatValue(SPEED, speed);
}
void Message::addSpeed(int speed){
    addFloatValue(SPEED, (float)speed);
}
void Message::addTime(float time){
    addFloatValue(TIME, time);
}
void Message::addTime(int time){
    addFloatValue(TIME, (float)time);
}
void Message::addVoltage(float voltage){
    addFloatValue(VOLTAGE, voltage);
}
void Message::addVoltage(int voltage){
    addFloatValue(VOLTAGE, (float)voltage);
}
void Message::addVolume(float volume){
    addFloatValue(VOLUME, volume);
}
void Message::addVolume(int volume){
    addFloatValue(VOLUME, (float)volume);
}
/* vim: set sw=4 ts=4 et: */
