#include "Base.h"

byte Base::broadCastAddress[4] = {BROADCAST_ADDR};
byte Base::baseAddress[4] = {BASE_ADDR};

void Base::configure(byte chipEnablePin, byte chipSelectPin) {
    NRF24::configure(chipEnablePin, chipSelectPin);
}

void Base::init()
{
    //Init the nrf24
    NRF24::init();
    setChannel(RF_CHANNEL);
    //set dynamic payload size
    setPayloadSize(0, 0);
    setPayloadSize(1, 0);
    //Set address size to 4
    setAddressSize(NRF24::NRF24AddressSize4Bytes);
    //Set CRC to 2 bytes
    setCRC(NRF24::NRF24CRC2Bytes);
    //Set 2 Mbps, maximum power
    setRF(NRF24::NRF24DataRate2Mbps, NRF24::NRF24TransmitPower0dBm);
    //Configure pipes
    setPipeAddress(0, broadCastAddress);
    enablePipe(0);
    disableAutoAck(0);

    setPipeAddress(1, baseAddress);
    enablePipe(1);
    enableAutoAck(1);

    //Configure retries
    setTXRetries(3, 3);

    //Starts listening
    powerUpRx();
}

boolean Base::sendToSensorino(byte address[], word service, byte* data, int len){
    setTransmitAddress(address, true);
    byte pkt[6+len];
    sensorino.composeBasePacket(pkt, service, data, len);
    return send(pkt, 6+len, false);
}

boolean Base::sendToBroadcast(word service, byte* data, int len){
    return sensorino.sendToBroadcast(service, data, len);
}

boolean Base::receive(unsigned int timeout, byte* pipe, byte* sender, unsigned int* service, byte* data, int* len){
    return sensorino.receive(timeout, pipe, sender, service, data, len);
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
    service.setTime(ts);
}

