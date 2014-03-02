/** Internals service: a service for sending information about the Sensorino.
 * This service works on the unicast channel (pipe 1).
 * Three data are supported now:
 * timestamp: the time of the node
 * temperature: internal temperature of the MCU
 * VCC: the voltage running in the MCU
 *
 * Author: Dario Salvi (dariosalvi78 at gmail dot com)
 *
 * Licensed under the GPL license http://www.gnu.org/copyleft/gpl.html
 */
#ifdef __cplusplus
extern "C"
#endif
#include "InternalsService.h"

boolean sendInternals(internalsPacket pkt){
    return sendToBase(INTERNALS_SERVICE, (byte*)&pkt, sizeof(internalsPacket));
}

internalsPacket parseInternals(byte* data){
    return *((internalsPacket *) data);
}

void serverSendInternals(byte* address, internalsPacket ints){
    Serial.print("{\"internals\"{ ");
    Serial.print("\"address\": [");
    Serial.print(address[0]);Serial.print(",");
    Serial.print(address[1]);Serial.print(",");
    Serial.print(address[2]);Serial.print(",");
    Serial.print(address[3]);
    Serial.print("], \"timestamp\": ");
    Serial.print(ints.timestamp);
    Serial.print(", \"temp\": ");
    Serial.print(ints.temperature);
    Serial.print(", \"volts\": ");
    Serial.print(((float)ints.vcc)/1000);
    Serial.println(" } }");
}

int readVcc() {
  long result;
  // Read 1.1V reference against AVcc
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  delay(2);
  ADCSRA |= _BV(ADSC);
  while (bit_is_set(ADCSRA,ADSC));
  result = ADCL;
  result |= ADCH<<8;
  result = 1126400L / result; // Back-calculate AVcc in mV
  return (int) result;
}

//From: http://playground.arduino.cc/Main/InternalTemperatureSensor
int readTemp() {
  ADMUX = (_BV(REFS1) | _BV(REFS0) | _BV(MUX3));
  ADCSRA |= _BV(ADEN);
  delay(20);
  ADCSRA |= _BV(ADSC);
  while (bit_is_set(ADCSRA,ADSC));
  return ADCW;
}
