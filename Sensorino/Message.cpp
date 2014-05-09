#include <Message.h>
#include <iostream>
#include<math.h>


typedef union {
  float f;
  struct {
    unsigned int mantisa : 23;
    unsigned int exponent : 8;
    unsigned int sign : 1;
  } parts;
} double_cast;



uint8_t Message::staticId=0;
Message::Message(uint8_t src[], uint8_t dst[])
{
 
    staticId=(staticId+1) % MAX_MESSAGE_ID;
    id=staticId;

    payloadLen=0;

    for (int i=0;i<4;i++){ 
        srcAddress[i]=src[i];
    }

    for (int i=0;i<4;i++){ 
        dstAddress[i]=dst[i];
    }

}

uint8_t Message::getId(){
    return id;
}

int _addInt(uint8_t *buffer, int value){
    // Value
    for (int i=0;i<sizeof(int);i++){
        buffer[sizeof(int)-1-i]=(value>>i*8)&0xFF;
    }
    return sizeof(int);
}


// Bozo tried to implement various float encoding but:
// -the python library he picked does not support binary representation
// http://www.strozhevsky.com/free_docs/asn1_in_simple_words.pdf
// -this implementation will procude literal representation of a float 
// (not fully tested) but it's a waste of space
// we will simply send the 4 bytes and decode them on the other arduino
int _addFloat_literalEncoding(uint8_t *buffer, float value){

    double_cast d1;
    d1.f = value;

    uint8_t firstByte=0b0000011;
    buffer[0]=firstByte;

    int exponent=0;

    int i=1;
    if (value<0){
        buffer[i++]=0x2D;
        value=-value;
    }

    while (value-(int)value>0){
        exponent-=1;
        value*=10;
    }

    int v=value;
    int e=exponent<0?-exponent:exponent;
    while (v>0 && i<50)
    {
        buffer[e+i]=0x30+v%10;
        e--;
        v=v/10;
    }
    int ee=exponent<0?-exponent:exponent;
    i+=ee;

    buffer[i++]=0x45;
    if (exponent<0)
    {
        buffer[i++]=0x2d;
        exponent=-exponent;
    }
    buffer[i++]=0x30+exponent;


    return i;
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

void Message::addFloat( float value){
    // Type
    payload[payloadLen++]=floatType; 

    double_cast d1;
    d1.f = value;

    // Len + Value
    int length=_addFloat_literalEncoding(payload+(payloadLen+1), value);
    payload[payloadLen]=length;
    payloadLen+=1+length;

    std::cout<<"payload final  is "<<payloadLen;

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


