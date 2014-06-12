#ifndef MESSAGE_H
#define MESSAGE_H

#define MAX_MESSAGE_ID 253

#include <stdint.h>
#include <cstddef>
#include <avr/pgmspace.h>

enum MessageType {
    ERR     = 0,
    GARBAGE = 1,
    PUBLISH = 2,
    SET     = 3,
    REQUEST = 4,
    ACK     = 5,
    STREAM  = 6,
    CONFIG  = 7,
};

#define DATATYPE_LIST_APPLY(F)	\
    /* Metatypes */\
    F(0, DATATYPE, DataType, int)\
    F(1, SERVICE_ID, ServiceId, int)\
    /* ISO-defined physical dimensions */\
    F(20, ACCELERATION, Acceleration, float)\
    F(21, AMOUNT, Amount, float)\
    F(22, ANGLE, Angle, float)\
    F(23, ANGULAR_VELOCITY, AngularVelocity, float)\
    F(24, AREA, Area, float)\
    F(25, RADIOACTIVITY, Radioactivity, float)\
    F(26, ELECTRICAL_CAPACITANCE, ElectricalCapacitance, float)\
    F(27, ELECTRICAL_RESISTANCE, ElectricalResistance, float)\
    F(28, ELECTRIC_CURRENT, ElectricCurrent, float)\
    F(29, ENERGY, Energy, float)\
    F(30, FORCE, Force, float)\
    F(31, FREQUENCY, Frequency, float)\
    F(32, ILLUMINANCE, Illuminance, float)\
    F(33, INDUCTANCE, Inductance, float)\
    F(34, LENGTH, Length, float)\
    F(35, LUMINOUS_FLUX, LuminousFlux, float)\
    F(36, LUMINOUS_INTENSITY, LuminousIntensity, float)\
    F(37, MAGNETIC_FIELD_STRENGTH, MagneticFieldStrength, float)\
    F(38, MASS, Mass, float)\
    F(39, POWER, Power, float)\
    F(40, PRESSURE, Pressure, float)\
    F(41, RELATIVE_HUMIDITY, RelativeHumidity, float)\
    F(42, SPEED, Speed, float)\
    F(43, TEMPERATURE, Temperature, float)\
    F(44, TIME, Time, float)\
    F(45, VOLTAGE, Voltage, float)\
    F(46, VOLUME, Volume, float)\
    /* Other common types */\
    F(50, COUNT, Count, int)\
    F(51, PRESENCE, Presence, bool)\
    F(52, SWITCH, Switch, bool)

enum DataType {
#define CAPS_ENUM(intval, CAPS, Camel, coding) \
    CAPS = intval,
DATATYPE_LIST_APPLY(CAPS_ENUM)
};

enum CodingType {
    intCoding,
    floatCoding,
    boolCoding,
};

#define HEADERS_LENGTH 4
#define PAYLOAD_LENGTH 20

class Message {
    public:
        /* Use this when building a brand new message */
        Message(uint8_t srcAddress, uint8_t dstAddress);

        /* Use this to grok a message you received */
        Message(const uint8_t *raw, int len);

        static const prog_char *dataTypeToString(DataType t,
                CodingType *coding = NULL);
        static DataType stringToDataType(const char *str);

        /* NOTE: this blocks */
        bool send(void);

        /* Header accessors */
        uint8_t getId();
        void setId(uint8_t id);

        uint8_t getSrcAddress();
        void setSrcAddress(uint8_t addr);
        uint8_t getDstAddress();
        void setDstAddress(uint8_t addr);

        MessageType getType(void);
        void setType(MessageType t);

        /* Raw accessors */
        const uint8_t *getRawData(void);
        int getRawLength(void);

        /* Payload accessors */

        /* Find @num'th TLV of type @t, decode it and store in @value,
         * @return non-zero on success.
         */
        int find(DataType t, int num, void *value);

        void addFloatValue(DataType t, float value);
        void addIntValue(DataType t, int value);
        void addDataTypeValue(DataType t);
        void addBoolValue(DataType t, int value);

        /* Accessors for types encoded as floats */
#define _glue(x, y)  x##y
#define glue(...) _glue(__VA_ARGS__)
#define int(...)
#define bool(...)
#define float(CamelName) \
        void glue(add, CamelName)(float val); \
        void glue(add, CamelName)(int val);
#define FLOAT_INT_ACCESSOR(intval, CAPS, Camel, coding) coding(Camel)
DATATYPE_LIST_APPLY(FLOAT_INT_ACCESSOR)
#undef float
#undef bool
#undef int

        /* Payload C++-like iterator */
        typedef uint8_t iter;
        iter begin();
        void iterAdvance(iter &i);
        void iterGetTypeValue(iter i, DataType *type, void *val);

    protected:
        uint8_t raw[HEADERS_LENGTH + PAYLOAD_LENGTH];
        uint8_t rawLen;

        static uint8_t staticId;

        void checkIntegrity();
};

#endif // whole file
/* vim: set sw=4 ts=4 et: */
