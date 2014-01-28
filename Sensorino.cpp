#include "Sensorino.h"
#include "NRF24.h"

Sensorino::Sensorino(uint8_t chipEnablePin, uint8_t chipSelectPin,
                     char* broadcastAdd, char* thisAdd)
                     : NRF24 (chipEnablePin, chipSelectPin)
{
    broadCastAddress = broadcastAdd;
    thisAddress = thisAdd;

}

Sensorino::Sensorino(char* thisAdd)
                     : NRF24 (9, 10)
{
    broadCastAddress = "BRCS";
    thisAddress = thisAdd;
}

boolean Sensorino::init()
{
    //Init the nrf24
    NRF24::init();
    //set addresses
    //setThisAddress(reinterpret_cast<uint8*>(thisAdd),3);
}

int Sensorino::readVcc() {
  long result;
  // Read 1.1V reference against AVcc
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Convert
  while (bit_is_set(ADCSRA,ADSC));
  result = ADCL;
  result |= ADCH<<8;
  result = 1126400L / result; // Back-calculate AVcc in mV
  return (int) result;
}


int Sensorino::readTemp() {
  long result;
  // Read temperature sensor against 1.1V reference
  ADMUX = _BV(REFS1) | _BV(REFS0) | _BV(MUX3);
  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Convert
  while (bit_is_set(ADCSRA,ADSC));
  result = ADCL;
  result |= ADCH<<8;
  result = (result - 125) * 1075;
  return (int) result;
}
