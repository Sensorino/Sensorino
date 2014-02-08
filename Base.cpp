/** Base, a base for sensorionos.
 * Adds communication to a server on the serial line.
 * Serial line messages are either debugging messages, either JSON messages.
 * JSON messages always start with { and end with }
 * In principle JSON messages are sent on one line.
 *
 * Author: Dario Salvi (dariosalvi78 at gmail dot com)
 *
 * Licensed under the GPL license http://www.gnu.org/copyleft/gpl.html
 */
#ifdef __cplusplus
extern "C"
#endif

#include <Base.h>

void configureBase(byte chipEnablePin, byte chipSelectPin, byte irqPin) {
    nRF24.configure(chipEnablePin, chipSelectPin, irqPin);
}

boolean startBase(){
    //Init the nrf24
    nRF24.init();
    if(!nRF24.setChannel(RF_CHANNEL))return false;
    //set dynamic payload size
    if(!nRF24.setPayloadSize(0, 0))return false;
    if(!nRF24.setPayloadSize(1, 0))return false;
    //Set address size to 4
    if(!nRF24.setAddressSize(NRF24::NRF24AddressSize4Bytes))return false;
    //Set CRC to 2 bytes
    if(!nRF24.setCRC(NRF24::NRF24CRC2Bytes))return false;
    //Set 2 Mbps, maximum power
    if(!nRF24.setRF(NRF24::NRF24DataRate2Mbps, NRF24::NRF24TransmitPower0dBm))return false;
    //Configure pipes
    if(!nRF24.setPipeAddress(0, broadCastAddress))return false;
    if(!nRF24.enablePipe(0))return false;
    if(!nRF24.setAutoAck(0, false))return false;

    if(!nRF24.setPipeAddress(1, baseAddress))return false;
    if(!nRF24.enablePipe(1))return false;
    if(!nRF24.setAutoAck(1, true))return false;

    //Configure retries
    if(!nRF24.setTXRetries(3, 3))return false;

    //Starts listening
    if(!nRF24.powerUpRx())return false;

    return true;
}

boolean sendToSensorino(byte address[], word service, byte* data, int len){
    nRF24.setTransmitAddress(address);
    byte pkt[6+len];
    composeBasePacket(pkt, service, data, len);
    return nRF24.send(pkt, 6+len, false);
}


String readLineFromSerial(char* buffer){
    int chars = Serial.readBytesUntil('\n', buffer, 100);
    char stringbuff[chars+1];
    for(int i=0; i<chars; i++)
        stringbuff[i]= buffer[i];
    stringbuff[chars] = '\0';
    return String(stringbuff);
}
