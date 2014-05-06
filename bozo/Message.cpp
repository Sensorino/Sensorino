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

int _addFloat(uint8_t *buffer, float value){

    double_cast d1;
    d1.f = value;

    std::cout<<"value is  "<<value<<"\n";

    // first byte is encoding information
    // binary encoding => bit 8 =1  
    // bit 7 is sign, 0 if > 0
    // base 2 => bit 6 to 5 = 00
    // Bits 4 to 3 encode the value of the binary scaling factor F as an unsigned binary integer
    // Bits 2 to 1 encode how many bytes the exponent occupies, 00 => second content octet encodes the value of the exponent
    uint8_t firstByte=0b10000000;
    if (d1.parts.sign==1){
        firstByte+=0b01000000;
    }
    buffer[0]=firstByte;

    // second byte will be exponent

// let's work on value
    int x=(int)value;
    float rest=value-x;
    buffer[1]=0;
    int i=0;
    int intRest=0;
    while (rest>0 && i<8)
    {
        std::cout << "rest is  = "<<rest;
        rest*=2;
        if (rest>=1)
        {
            intRest=intRest|0x01<<i;
            rest=rest-1;
        }
        i++;
    }
    std::cout << "we stop at exponent = "<<i;
    buffer[1]=i+127;

    std::cout << "int rest : "<<intRest<<" x was : "<<x<<"\n";


    //let's find required shift
    int shift=0;

    while (shift < 255 &&  x>>shift>2){
        std::cout << "shifted: "<<(x<<shift)<<"\n";
        shift++;
    }

    std::cout << "we should shift "<<shift <<"times\n";
    x=x<<shift+intRest;

    std::cout <<" x is : "<<x<<"\n";

    // integer part
    for (int i=0;i<3;i++){
        //buffer[4-i]=(d1.parts.mantisa>>i*8)&0xFF;
        buffer[i+2]=(d1.parts.mantisa<<i*8)&0xFF;
//        std::cout <<"p"<<i<<":"<<(uint8_t)((d1.parts.mantisa<<i*8)&0xFF);
//        buffer[2+i]=x>>(i*8)&0xFF;
    }


    /*

    std::cout<<"exponent is  "<<d1.parts.exponent;
    buffer[1]=d1.parts.exponent;
    
    
    std::cout<<" mantissa is  "<<d1.parts.mantisa;
    std::cout<<" sign is  "<<d1.parts.sign;
    // Value
    for (int i=0;i<3;i++){
        //buffer[4-i]=(d1.parts.mantisa>>i*8)&0xFF;
        buffer[2+i]=(d1.parts.mantisa<<i*8)&0xFF;
    }
    */
    return 5;
}


void Message::addFloatValue(DataType t, float value){
    // Type
    payload[payloadLen++]=extendedType; // extended type
    payload[payloadLen++]=t;

    std::cout<<"payload with ext is "<<payloadLen;
    // Len and value
    addFloat(value);
    /*
    int length=_addFloat(payload+payloadLen+1, value);
    payload[payloadLen]=length;
    payloadLen+=1+length;
    */
}

void Message::addFloat( float value){
    // Type
    payload[payloadLen++]=floatType; 

    std::cout<<"payload with f is "<<payloadLen;
    // Len + Value
    int length=_addFloat(payload+payloadLen+1, value);
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






void Message::addTemperature(float temp){
    addFloatValue(TEMPERATURE, temp);
}

void Message::addTemperature(int temp){
    addFloatValue(TEMPERATURE, (float)temp);
}



