#ifndef MESSAGE_H
#define MESSAGE_H

#define MAX_MESSAGE_ID 253

#include <stdint.h>
#include <cstddef>
#include <avr/pgmspace.h>

#include "SensorinoUtils.h"

#define DATATYPE_LIST_APPLY(F)	\
    /* Metatypes */\
    F(0, DATATYPE, DataType, int)\
    F(1, SERVICE_ID, ServiceId, int)\
    F(2, MESSAGE, Message, binary)\
    F(3, EXPRESSION, Expression, binary)\
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
    F(52, SWITCH, Switch, bool)\
    F(53, COLOR_COMPONENT, ColorComponent, float)

namespace Data {
    enum Type {
#define CAPS_ENUM(intval, CAPS, Camel, coding) \
        CAPS = intval,
DATATYPE_LIST_APPLY(CAPS_ENUM)
    };
};

#define HEADERS_LENGTH 4
#define PAYLOAD_LENGTH 80

#define MAX_MESSAGE_SIZE (HEADERS_LENGTH + PAYLOAD_LENGTH)

class Message {
    public:
        enum Type {
            ERR     = 0,
            GARBAGE = 1,
            PUBLISH = 2,
            SET     = 3,
            REQUEST = 4,
        };

        enum CodingType {
            intCoding,
            floatCoding,
            boolCoding,
            binaryCoding,
        };

        struct BinaryValue {
            uint8_t *value;
            uint8_t len;
        };

        /* Use this when building a brand new message */
        Message(uint8_t srcAddress, uint8_t dstAddress);

        /* Use this to grok a message you received */
        Message(const uint8_t *raw, int len);

        /* Use this to obtain a receive buffer for a new message */
        Message();

        static const prog_char *dataTypeToString(Data::Type t,
                CodingType *coding = NULL);
        static Data::Type stringToDataType(const char *str);

        /* NOTE: this blocks */
        bool send(void);

        /* Header accessors */
        uint8_t getId();
        void setId(uint8_t id);

        uint8_t getSrcAddress();
        void setSrcAddress(uint8_t addr);
        uint8_t getDstAddress();
        void setDstAddress(uint8_t addr);

        Type getType(void);
        void setType(Type t);

        /* Raw accessors */
        const uint8_t *getRawData(void);
        int getRawLength(void);

        uint8_t *getWriteBuffer(void);
        void writeLength(int len);

        /* Payload accessors */

        /* Find @num'th TLV of type @t, decode it and store in @value,
         * @return non-zero on success.
         */
        int find(Data::Type t, int num, void *value);

        void addFloatValue(Data::Type t, float value);
        void addIntValue(Data::Type t, int value);
        void addDataTypeValue(Data::Type t);
        void addBoolValue(Data::Type t, bool value);
        void addBinaryValue(Data::Type t, const uint8_t *value, uint8_t len);

        /* Accessors for types encoded as floats */
#define int(...)
#define bool(...)
#define float(CamelName) \
        void glue(add, CamelName)(float val); \
        void glue(add, CamelName)(int val);
#define binary(...)
#define FLOAT_INT_ACCESSOR(intval, CAPS, Camel, coding) coding(Camel)
DATATYPE_LIST_APPLY(FLOAT_INT_ACCESSOR)
#undef binary
#undef float
#undef bool
#undef int

        /* Payload C++-like iterator */
        typedef uint8_t iter;
        iter begin();
        void iterAdvance(iter &i);
        void iterGetTypeValue(iter i, Data::Type *type, void *val);

        static float toFloat(Data::Type t, void *val);

    protected:
        uint8_t raw[HEADERS_LENGTH + PAYLOAD_LENGTH];
        uint8_t rawLen;

        static uint8_t staticId;

        void checkIntegrity();
};

#endif // whole file
/* vim: set sw=4 ts=4 et: */
