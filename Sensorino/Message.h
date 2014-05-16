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

#define HEADERS_LENGTH 8
#define PAYLOAD_LENGTH 20


class Message{

    public:
        Message(uint8_t srcAddress, uint8_t dstAddress);
        Message(uint8_t srcAddress, uint8_t dstAddress, uint8_t id);

        void addFloatValue(DataType t, float value);
        void addIntValue(DataType t, int value);

        const char *dataTypeToString(DataType t);
        const char *msgTypeAsString();
    

        // there should be no reason to do this, but I need to test
        void addFloat(float value);
        void addInt(int value);

        

        void addAcceleration(float acceleration);
        void addAcceleration(int acceleration);
        void addAmount(float amount);
        void addAmount(int amount);
        void addAngle(float angle);
        void addAngle(int angle);
        void addAngular_velocity(float velocity);
        void addAngular_velocity(int velocity);
        void addArea(float area);
        void addArea(int area);
        void addElectrical_capacitance(float capacitance);
        void addElectrical_capacitance(int capacitance);
        void addElectrical_resistance(float resistance);
        void addElectrical_resistance(int resistance);
        void addElectric_current(float current);
        void addElectric_current(int current);
        void addEnergy(float energy);
        void addEnergy(int energy);
        void addForce(float force);
        void addForce(int force);
        void addFrequency(float frequency);
        void addFrequency(int frequency);
        void addHumidity(float relative_humidity);
        void addHumidity(int relative_humidity);
        void addIlluminance(float illuminance);
        void addIlluminance(int illuminance);
        void addInductance(float inductance);
        void addInductance(int inductance);
        void addLength(float length);
        void addLength(int length);
        void addLuminous_flux(float luminous_flux);
        void addLuminous_flux(int luminous_flux);
        void addLuminous_intensity(float luminous_intensity);
        void addLuminous_intensity(int luminous_intensity);
        void addMagnetic_field_strength(float magnetic_field_strength);
        void addMagnetic_field_strength(int magnetic_field_strength);
        void addMass(float mass);
        void addMass(int mass);
        void addPower(float power);
        void addPower(int power);
        void addPressure(float pressure);
        void addPressure(int pressure);
        void addRadioactivity(float radioactivity);
        void addRadioactivity(int radioactivity);
        void addSpeed(float speed);
        void addSpeed(int speed);
        void addTemperature(float temperature);
        void addTemperature(int temperature);
        void addTime(float time);
        void addTime(int time);
        void addVoltage(float voltage);
        void addVoltage(int voltage);
        void addVolume(float volume);
        void addVolume(int volume);


        uint8_t getId();
        void setId(uint8_t id);

        uint8_t getDstAddress();    
        
        inline uint8_t* getRawData();

        inline int getPayloadLength();
        inline uint8_t* getPayload();
        inline void setPayload(uint8_t *data, uint8_t len);
        

        MessageType getType();
        void setType(MessageType t);

        void decode(uint8_t *data, uint8_t len, uint8_t id);

        

            

    protected:
    uint8_t payload[PAYLOAD_LENGTH];
    uint8_t payloadLen;

    uint8_t srcAddress;
    uint8_t dstAddress;
    uint8_t id;

    //uint8_t static staticId;

    MessageType messageType;
};


int Message::getPayloadLength(){
    return payloadLen;
}

uint8_t* Message::getPayload(){
    return payload;
}


#endif // whole file
