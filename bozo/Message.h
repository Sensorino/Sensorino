#ifndef MESSAGE_H
#define MESSAGE_H

#define MAX_MESSAGE_ID 253

#include <stdint.h>


enum MessageType {
        ERR     = 0,
        CTRL    = 1,
        PUBLISH = 2,
        SET     = 3,
        REQUEST = 4,
        ACK     = 5,
        STREAM  = 6,
        CONFIG  = 7,
};

enum DataType {
    ACCELERATION            = 1,
    AMOUNT                  = 2,
    ANGLE                   = 3,
    ANGULAR_VELOCITY        = 4,
    AREA                    = 5,
    RADIOACTIVITY           = 6,
    ELECTRICAL_CAPACITANCE  = 7,
    ELECTRICAL_RESISTANCE   = 8,
    ELECTRIC_CURRENT        = 9,
    ENERGY                  = 10,
    FORCE                   = 11,
    FREQUENCY               = 12,
    ILLUMINANCE             = 13,
    INDUCTANCE              = 14,
    LENGTH                  = 15,
    LUMINOUS_FLUX           = 16,
    LUMINOUS_INTENSITY      = 17,
    MAGNETIC_FIELD_STRENGTH = 18,
    MASS                    = 19,
    POWER                   = 20,
    PRESSURE                = 21,
    RELATIVE_HUMIDITY       = 22,
    SPEED                   = 23,
    TEMPERATURE             = 24,
    TIME                    = 25,
    VOLTAGE                 = 26,
    VOLUME                  = 27,
};




// https://en.wikipedia.org/wiki/Basic_Encoding_Rules#BER_encoding
const uint8_t extendedType=0b00011111;
const uint8_t booleanType=1;
const uint8_t intType=2;
const uint8_t nullType=5;
const uint8_t floatType=9;
const uint8_t charStringType=29;




class Message{

    public:
        Message(uint8_t srcAddress[4], uint8_t dstAddress[4]);

        void addFloatValue(DataType t, float value);
        void addIntValue(DataType t, int value);

        // there should be no reason to do this, but I need to test
        void addFloat(float value);
        void addInt(int value);

        void addTemperature(float temp);
        void addTemperature(int temp);

        uint8_t getId();
        inline int getPayloadLength();
        inline uint8_t* getPayload();
            

    protected:
    uint8_t payload[20];
    uint8_t payloadLen;

    uint8_t srcAddress[4];
    uint8_t dstAddress[4];
    uint8_t id;
    uint8_t static staticId;

    uint8_t messageType;
};


int Message::getPayloadLength(){
    return payloadLen;
}

uint8_t* Message::getPayload(){
    return payload;
}


#endif // whole file
