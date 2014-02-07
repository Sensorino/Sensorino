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
 #include "Base.h"

void Base::configure(byte chipEnablePin, byte chipSelectPin) {
    NRF24::configure(chipEnablePin, chipSelectPin);
}

boolean Base::init()
{
    //Init the nrf24
    NRF24::init();
    if(!setChannel(RF_CHANNEL))return false;
    //set dynamic payload size
    if(!setPayloadSize(0, 0))return false;
    if(!setPayloadSize(1, 0))return false;
    //Set address size to 4
    if(!setAddressSize(NRF24::NRF24AddressSize4Bytes))return false;
    //Set CRC to 2 bytes
    if(!setCRC(NRF24::NRF24CRC2Bytes))return false;
    //Set 2 Mbps, maximum power
    if(!setRF(NRF24::NRF24DataRate2Mbps, NRF24::NRF24TransmitPower0dBm))return false;
    //Configure pipes
    if(!setPipeAddress(0, broadCastAddress))return false;
    if(!enablePipe(0))return false;
    if(!setAutoAck(0, false))return false;

    if(!setPipeAddress(1, baseAddress))return false;
    if(!enablePipe(1))return false;
    if(!setAutoAck(1, true))return false;

    //Configure retries
    if(!setTXRetries(3, 3))return false;

    //Starts listening
    if(!powerUpRx())return false;

    return true;
}

boolean Base::sendToSensorino(byte address[], word service, byte* data, int len){
    setTransmitAddress(address);
    byte pkt[6+len];
    composeBasePacket(pkt, service, data, len);
    return send(pkt, 6+len, false);
}


String Base::readLineFromSerial(char* buffer){
    int chars = Serial.readBytesUntil('\n', buffer, 100);
    char stringbuff[chars+1];
    for(int i=0; i<chars; i++)
        stringbuff[i]= buffer[i];
    stringbuff[chars] = '\0';
    return String(stringbuff);
}

void Base::parseServerLine(String line){
    if(line.startsWith("#time")){
        parseServerTime(line);
    }
}

//Time protocol:
void Base::askServerTime(){
    Serial.println("#getTime");
    char buff[100];
    String timestring = readLineFromSerial(buff);
}

void Base::parseServerTime(String line){
    String number = line.substring(6);
    int len = line.length();
    char buff[len+1];
    line.toCharArray(buff, len);
    buff[len]='\0';
    unsigned long ts = strtoul(buff, NULL, 0);
    setTime(ts);
}

//Internals protocol:
void Base::serverSendInternals(byte* address, internalsPacket ints){
    Serial.println("#internals");
    Serial.print("#address: ");
    Serial.print(address[0]);Serial.print(".");
    Serial.print(address[1]);Serial.print(".");
    Serial.print(address[2]);Serial.print(".");
    Serial.print(address[3]);
    Serial.print(" temp: ");
    Serial.print(((float)ints.temp)/1000);
    Serial.print(" volts: ");
    Serial.print(((float)ints.vcc)/1000);
    Serial.println();
}

