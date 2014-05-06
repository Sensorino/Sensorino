#include <Message.h>
#include <iostream>
#include <iomanip>

// http://stackoverflow.com/questions/5702931/convert-integer-decimal-to-hex-on-an-arduino
// Serial.print(0x55, HEX);


void binary(uint8_t number) {
    int remainder;

    for (int i=7; i>=0; i--) {
        int bit = ((number >> i) & 1);
        std::cout << bit;
    }
}

typedef union {
  float f;
  struct {
    unsigned int mantisa : 23;
    unsigned int exponent : 8;
    unsigned int sign : 1;
  } parts;
} double_cast;


int main(){

  uint8_t srcAddress[4];
  srcAddress[0]=1;
  srcAddress[1]=2;
  srcAddress[2]=3;
  srcAddress[3]=4;

  uint8_t dstAddress[4];
  dstAddress[0]=1;
  dstAddress[1]=2;
  dstAddress[2]=3;
  dstAddress[3]=4;


  std::cout << "salut\n\n";


  Message m=Message(srcAddress, dstAddress);
//  m.addTemperature(1);
//  m.addIntValue(TEMPERATURE, 22.2);


    double_cast d1;
    d1.f = 12.375;
    std::cout<<" mantissa is  "<<d1.parts.mantisa;
        std::cout<<" exponent is  "<<d1.parts.exponent;
    std::cout<<"\n";

  m.addFloat(12.375);



  std::cout << "payload is now " <<  m.getPayloadLength()<<"\n";
  for(int i=0;i<m.getPayloadLength();i++){
    binary(m.getPayload()[i]);
    std::cout << " hex:";
    std::cout << std::hex << std::uppercase << (int)m.getPayload()[i];
    std::cout << std::dec << " dec "<<(int)m.getPayload()[i];
    std::cout << "\n";
    
  }

  
  std::cout << "\n";
  std::cout << "\n";
  std::cout << "\n";

  for(int i=0;i<m.getPayloadLength();i++){
      std::cout.fill('0');
      std::cout.width(2);
      std::cout << std::hex << std::uppercase << (int)m.getPayload()[i]<<"\\x";
  }
  std::cout << "\n";

    
}



