/** Sensorino library.
 * This library abstracts the nRF24L01.
 * Decisions taken:
 * - pipe 0 is used as broadcast pipe, with shared address and no acks
 * - pipe 1 is used as private address
 * - nodes send their address
 * - addresses are 4 bytes long
 * - CRC is 2 bytes
 * - 2Mbps, 750us ack time, 3 retries
 * The library also implements a set of "services" on top of basic communication means.
 *
 * Author: Dario Salvi (dariosalvi78 at gmail dot com)
 *
 * Licensed under the GPL license http://www.gnu.org/copyleft/gpl.html
 */
 #include "Sensorino.h"

unsigned long Sensorino::lastUnixTime = 0;
unsigned long Sensorino::lastTimeStamp = 0;

unsigned long Sensorino::getTime(){
    if(lastUnixTime != 0)
        return lastUnixTime + ( (millis() - lastTimeStamp)/1000 );
    else return 0;
}

void Sensorino::setTime(unsigned long ts){
    lastTimeStamp = millis();
    lastUnixTime = ts;
}

void Sensorino::askTime(){
    byte message[4] = {0,0,0,0}; //zeros message = "ask for time"
    if(!sensorino.sendToBroadcast(TIME_SERVICE, message, 4))
        return;
    unsigned int receivedService = 0;
    byte receivedMessage[30];
    byte sender[4];
    int receivedLen = 0;
    byte pipe;
    if(sensorino.receive(2000, &pipe, sender,
                      &receivedService, receivedMessage, &receivedLen)) {
        if((pipe == BROADCAST_PIPE) && (receivedService == TIME_SERVICE) && (receivedLen>=4)){
            timePacket pkt = *((timePacket *) receivedMessage);
            setTime( pkt.timestamp );
            //setTime( *((unsigned long *)receivedMessage) );
        }
    }
}

void Sensorino::serveTime(){
    unsigned long t = getTime();
    if(t != 0){
        /**byte message[4];
        message[0]= t & 0xFF;
        message[1]= (t >> 8) & 0xFF;
        message[2]= (t >> 16) & 0xFF;
        message[3]= (t >> 24) & 0xFF;
        sensorino.sendToBroadcast(TIME_SERVICE, message, 4);*/
        timePacket pkt;
        pkt.timestamp =t;
        sensorino.sendToBroadcast(TIME_SERVICE, (byte *) &pkt, sizeof(pkt));
    }
}

void Sensorino::sendInternals(){
    internalsPacket pkt;
    pkt.vcc = readVcc();
    pkt.temp = readTemp();
    sensorino.sendToBase(INTERNALS_SERVICE, (byte*)&pkt, 4);
}

internalsPacket Sensorino::parseInternals(byte* data){
    return *((internalsPacket *) data);
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
