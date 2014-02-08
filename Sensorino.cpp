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
#include "NRF24.h"

// Default addresses:
byte Sensorino::broadCastAddress[4] = {BROADCAST_ADDR};
byte Sensorino::baseAddress[4] = {BASE_ADDR};
byte Sensorino::thisAddress[4] = {1,2,3,4};

void Sensorino::configure(byte chipEnablePin, byte chipSelectPin, byte irqPin, byte myAdd[]) {
    NRF24::configure(chipEnablePin, chipSelectPin, irqPin);
    thisAddress[0] = myAdd[0];
    thisAddress[1] = myAdd[1];
    thisAddress[2] = myAdd[2];
    thisAddress[3] = myAdd[3];
}

boolean Sensorino::init()
{
    //Init the nrf24
    NRF24::init();
    if(!setChannel(RF_CHANNEL)) return false;
    //set dynamic payload size
    if(!setPayloadSize(0, 0)) return false;
    if(!setPayloadSize(1, 0)) return false;
    //Set address size to 4
    if(!setAddressSize(NRF24::NRF24AddressSize4Bytes)) return false;
    //Set CRC to 2 bytes
    if(!setCRC(NRF24::NRF24CRC2Bytes)) return false;
    //Set 2 Mbps, maximum power
    if(!setRF(NRF24::NRF24DataRate2Mbps, NRF24::NRF24TransmitPower0dBm)) return false;
    //Configure pipes
    if(!setPipeAddress(0, broadCastAddress)) return false;
    if(!enablePipe(0)) return false;
    if(!setAutoAck(0, false)) return false;
    if(!setPipeAddress(1, thisAddress)) return false;
    if(!enablePipe(1)) return false;
    if(!setAutoAck(1, true)) return false;
    //Configure retries
    if(!setTXRetries(3, 3)) return false;
    return true;
}


boolean Sensorino::sendToBase(unsigned int service, byte* data, int len){
    if(!setTransmitAddress(baseAddress))
        return false;
    byte pkt[6+len];
    composeBasePacket(pkt, service, data, len);
    return send(pkt, 6+len, false);
}

boolean Sensorino::sendToBroadcast(unsigned int service, byte* data, int len){
    if(!setTransmitAddress(broadCastAddress))
        return false;
    byte pkt[6+len];
    composeBasePacket(pkt, service, data, len);
    return send(pkt, 6+len, true);
}


boolean Sensorino::receive(unsigned int timeout, byte* pipe, byte* sender,
                           unsigned int* service, byte* data, int* len){
    byte buffer[NRF24_MAX_MESSAGE_LEN];
    byte totlen;
    if(waitAvailableTimeout(timeout)){
            if(recv(pipe, buffer, &totlen)){
                decomposeBasePacket(buffer, totlen, sender, service, data, len);
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
}

void Sensorino::decomposeBasePacket(byte* packet, int totlen, byte* sender,
                                    unsigned int* service, byte* data, int* len){
    sender[0] = packet[0];
    sender[1] = packet[1];
    sender[2] = packet[2];
    sender[3] = packet[3];
    *service = (unsigned int)(packet[5] <<8) + (unsigned int)packet[4];
    *len = totlen -6;
    for(int i=0; i<*len; i++){
     data[i] = packet[i+6];
    }
}

