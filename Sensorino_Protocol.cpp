/** Sensorino communication library.
 * This library goes on top of the nRF24L01.
 * Decisions taken:
 * - pipe 0 is used as broadcast pipe, with shared address and no acks
 * - pipe 1 is used as private address
 * - nodes send their address
 * - addresses are 4 bytes long
 * - CRC is 2 bytes
 * - 2Mbps, 750us ack time, 3 retries
 *
 * Author: Dario Salvi (dariosalvi78 at gmail dot com)
 *
 * Licensed under the GPL license http://www.gnu.org/copyleft/gpl.html
 */
#ifdef __cplusplus
extern "C"
#endif

#include <Sensorino_Protocol.h>

//Addresses:
byte broadCastAddress[4] = {BROADCAST_ADDR};
byte baseAddress[4] = {BASE_ADDR};
byte thisAddress[4] = {1,2,3,4}; //just a default address

boolean startRadio(byte chipEnablePin, byte chipSelectPin, byte irqPin, byte myAdd[]) {
    NRF24::configure(chipEnablePin, chipSelectPin, irqPin);
    thisAddress[0] = myAdd[0];
    thisAddress[1] = myAdd[1];
    thisAddress[2] = myAdd[2];
    thisAddress[3] = myAdd[3];

    //Init the nrf24
    nRF24.init();
    if(!nRF24.setChannel(RF_CHANNEL)) return false;
    //set dynamic payload size
    if(!nRF24.setPayloadSize(0, 0)) return false;
    if(!nRF24.setPayloadSize(1, 0)) return false;
    //Set address size to 4
    if(!nRF24.setAddressSize(NRF24::NRF24AddressSize4Bytes)) return false;
    //Set CRC to 2 bytes
    if(!nRF24.setCRC(NRF24::NRF24CRC2Bytes)) return false;
    //Set 2 Mbps, maximum power
    if(!nRF24.setRF(NRF24::NRF24DataRate2Mbps, NRF24::NRF24TransmitPower0dBm)) return false;
    //Configure pipes
    if(!nRF24.setPipeAddress(0, broadCastAddress)) return false;
    if(!nRF24.enablePipe(0)) return false;
    if(!nRF24.setAutoAck(0, false)) return false;
    if(!nRF24.setPipeAddress(1, thisAddress)) return false;
    if(!nRF24.enablePipe(1)) return false;
    if(!nRF24.setAutoAck(1, true)) return false;
    //Configure retries
    if(!nRF24.setTXRetries(3, 3)) return false;
    return true;
}

boolean send(boolean broadcast, byte* destination, MessageType msgType, byte* data, int len){
    if(broadcast){
        if(!nRF24.setTransmitAddress(broadCastAddress)) return false;
    }
    else{
        if(!nRF24.setTransmitAddress(destination)) return false;
    }
    unsigned int totlen = len + 7;
    byte pkt[totlen];
    pkt[0] = thisAddress[0];
    pkt[1] = thisAddress[1];
    pkt[2] = thisAddress[2];
    pkt[3] = thisAddress[3];
    pkt[4] = msgType;
    for(int i=0; i<len; i++){
        pkt[i+5] = data[i];
    }
    return nRF24.send(pkt, totlen, broadcast);
}

boolean receive(unsigned int timeoutMS, boolean* broadcast, byte* sender, MessageType* msgType,
                byte* databuffer, int* len){
    byte buffer[NRF24_MAX_MESSAGE_LEN];
    byte totlen;
    byte pipe;
    if(nRF24.waitAvailableTimeout(timeoutMS)){
            if(nRF24.recv(&pipe, buffer, &totlen)){

                *broadcast = (pipe == BROADCAST_PIPE);
                sender[0] = buffer[0];
                sender[1] = buffer[1];
                sender[2] = buffer[2];
                sender[3] = buffer[3];
                *msgType = (MessageType) buffer[4];
                *len = totlen - 5;
                for(int i=0; i<*len; i++)
                    databuffer[i] = buffer[i+5];
                return true;
            }
    }
    return false;

}


void makeCtrlPayload(byte* payload, ControlType controlT, byte* data, int len){
    payload[0] = controlT;
    for(int i=0; i< len;i++){
        payload[i+1] = data[i];
    }
}

boolean sendControl(boolean broadcast, byte* destination, ControlType controlT, byte* data, int len){
    byte ctrlpl[len +1];
    makeCtrlPayload(ctrlpl, controlT, data, len);
    send(broadcast, destination, CTRL, ctrlpl, len+1);
}

void unmakeCtrlPayload(byte* ctrlpayload, int totlen, ControlType* controlT, byte* data, int* len){
    *controlT = (ControlType) ctrlpayload[0];
    *len = totlen-1;
    for(int i=0; i<*len; i++){
        data[i] = ctrlpayload[i+1];
    }
}

void makeErrorPayload(byte* payload, ErrorType errT, byte* data, int len){
    payload[0] = errT;
    for(int i=0; i< len;i++){
        payload[i+1] = data[i];
    }
}

boolean sendError(boolean broadcast, byte* destination, ErrorType errorT, byte* data, int len){
    byte errpl[len +1];
    makeErrorPayload(errpl, errorT, data, len);
    send(broadcast, destination, ERR, errpl, len+1);
}

void unmakeErrorPayload(byte* errpayload, int totlen, ErrorType* errT, byte* data, int* len){
    *errT = (ErrorType) errpayload[0];
    *len = totlen-1;
    for(int i=0; i<*len; i++){
        data[i] = errpayload[i+1];
    }
}


void makeServicePayload(byte* payload, unsigned int serviceType, byte serviceInstance,
                        DataFormat dataF, byte* data, int len){
    payload[0] = serviceType & 0xFF ;
    payload[1] = (serviceType >> 8) & 0xFF;
    payload[2] = serviceInstance;
    payload[3] = dataF;
    for(int i=0; i<len; i++){
        payload[i+4] = data[i];
    }
}

boolean sendService(boolean broadcast, byte* destination, unsigned int serviceType, byte serviceInstance,
                    MessageType msgt, DataFormat dataF, byte* data, int len){
    byte servpl[len +4];
    makeServicePayload(servpl, serviceType, serviceInstance, dataF, data, len);
    send(broadcast, destination, msgt, servpl, len+1);
}

void unmakeServicePayload(byte* payload, int totlen, unsigned int* serviceType,
                            byte* serviceInstance, DataFormat* dataF, byte* data, int* len){
    *serviceType = (unsigned int)(payload[1] <<8) + (unsigned int)payload[0];
    *serviceInstance = payload[2];
    *dataF = (DataFormat) payload[3];
    *len = totlen-4;
    for(int i =0; i<*len; i++){
        data[i] = payload[i+4];
    }
}
