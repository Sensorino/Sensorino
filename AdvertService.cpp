/** A service for advertising the presence and the services of a Sensorino.
 * This service works on the unicast channel (pipe 1).
 * It sends a list of services identifiers.
 *
 * Author: Dario Salvi (dariosalvi78 at gmail dot com)
 *
 * Licensed under the GPL license http://www.gnu.org/copyleft/gpl.html
 */
#ifdef __cplusplus
extern "C"
#endif
#include "AdvertService.h"

boolean sendAdvert(advertPacket pkt){
    int size = 1 + (pkt.servicesNumber * sizeof(unsigned int));
    byte packet[size];
    packet[0] = pkt.servicesNumber;
    for(int i=0; i<pkt.servicesNumber; i++){
        packet[(2*i)-1] = pkt.services[i] & 0xFF ;
        packet[2*i] = (pkt.services[i] >> 8) & 0xFF;
    }
    return sendToBase(ADVERT_SERVICE, packet, size);
}

advertPacket parseAdvert(byte* data){
    advertPacket pkt;
    pkt.servicesNumber = data[0];
    for(int i=0; i<pkt.servicesNumber; i++){
        pkt.services[i]= (unsigned int)(data[2*i] <<8) + (unsigned int)data[(2*i)-1];
    }
    return pkt;
}

void serverSendAdvert(byte* address, advertPacket adv){
    Serial.print("{\"advert\"{ ");
    Serial.print("\"address\": [");
    Serial.print(address[0]);Serial.print(",");
    Serial.print(address[1]);Serial.print(",");
    Serial.print(address[2]);Serial.print(",");
    Serial.print(address[3]);
    Serial.print("], \"services\": [");
    for(int i=0; i<adv.servicesNumber-1; i++){
        Serial.print(adv.services[i]);
        Serial.print(",");
    }
    if(adv.servicesNumber > 0){
        Serial.print(adv.services[adv.servicesNumber-1]);
    }
    Serial.println("] } }");
}
