#include "Sensorino.h"
#include "NRF24.h"

/** Default addresses.
 */
byte Sensorino::broadCastAddress[4] = {BROADCAST_ADDR};
byte Sensorino::baseAddress[4] = {BASE_ADDR};
byte Sensorino::thisAddress[4] = {1,2,3,4};

void Sensorino::configure(byte chipEnablePin, byte chipSelectPin, byte myAdd[]) {
    NRF24::configure(chipEnablePin, chipSelectPin);
    thisAddress[0] = myAdd[0];
    thisAddress[1] = myAdd[1];
    thisAddress[2] = myAdd[2];
    thisAddress[3] = myAdd[3];
}

void Sensorino::init()
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

    setPipeAddress(1, thisAddress);
    enablePipe(1);
    enableAutoAck(1);

    //Configure retries
    setTXRetries(3, 3);
}


boolean Sensorino::sendToBase(unsigned int service, byte* data, int len){
    setTransmitAddress(baseAddress, true);
    byte pkt[6+len];
    composeBasePacket(pkt, service, data, len);
    return send(pkt, 6+len, false);
}

boolean Sensorino::sendToBroadcast(unsigned int service, byte* data, int len){
    setTransmitAddress(broadCastAddress, false);
    byte pkt[6+len];
    composeBasePacket(pkt, service, data, len);
    return send(pkt, 6+len, true);
}


boolean Sensorino::receive(unsigned int timeout, byte* pipe, byte* sender,
                           unsigned int* service, byte* data, int* len){
    byte buffer[NRF24_MAX_MESSAGE_LEN];
    byte totlen;
    if(waitAvailableTimeout(timeout)){
            if(sensorino.recv(pipe, buffer, &totlen)){
                sensorino.decomposeBasePacket(buffer, totlen, sender, service, data, len);
            return true;
        }
    }
    return false;
}


void Sensorino::composeBasePacket(byte* buffer, unsigned int service, byte* data, int len){
    int totlen = 6 + len;
    buffer[0] = thisAddress[0];
    buffer[1] = thisAddress[1];
    buffer[2] = thisAddress[2];
    buffer[3] = thisAddress[3];
    buffer[4] = service & 0xFF ;
    buffer[5] = (service >> 8) & 0xFF;
    for(int i=0; i<len;i++){
     buffer[i+6] = data[i];
    }

    Serial.print("composing packet: sender ");
    Serial.print(buffer[0]);Serial.print(".");
    Serial.print(buffer[1]);Serial.print(".");
    Serial.print(buffer[2]);Serial.print(".");
    Serial.print(buffer[3]);
    Serial.print(" service ");
    Serial.print(buffer[4], HEX);Serial.print(".");
    Serial.print(buffer[5], HEX);
    Serial.print(" data ");
    for(int i=0; i<len;i++){
     Serial.print(buffer[i+6]);Serial.print(".");
    }
    Serial.println();
}

void Sensorino::decomposeBasePacket(byte* packet, int totlen, byte* sender,
                                    unsigned int* service, byte* data, int* len){
    sender[0] = packet[0];
    sender[1] = packet[1];
    sender[2] = packet[2];
    sender[3] = packet[3];
    *service = (unsigned int)(packet[4] <<8) + (unsigned int)packet[5];
    *len = totlen -6;
    for(int i=0; i<*len; i++){
     data[i] = packet[i+6];
    }

    Serial.print("decomposing packet: sender ");
    Serial.print(sender[0]);Serial.print(".");
    Serial.print(sender[1]);Serial.print(".");
    Serial.print(sender[2]);Serial.print(".");
    Serial.print(sender[3]);
    Serial.print(" service ");
    Serial.print(packet[4], HEX);Serial.print(".");
    Serial.print(packet[5], HEX);
    for(int i=0; i<*len; i++){
     Serial.print(data[i]);Serial.print(".");
    }
    Serial.println();
}

