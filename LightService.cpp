/** Light intensity service: a service for sending information about brightness.
 * This service works on the unicast channel (pipe 1).
 * Two data are supported:
 * timestamp: the time of the node
 * light: the raw ADC value of the light intensity
 *
 * Author: Dario Salvi (dariosalvi78 at gmail dot com)
 *
 * Licensed under the GPL license http://www.gnu.org/copyleft/gpl.html
 */
#ifdef __cplusplus
extern "C"
#endif
#include "LightService.h"

boolean sendLight(lightPacket pkt){
    return sendToBase(LIGHT_SERVICE, (byte*)&pkt, sizeof(lightPacket));
}


lightPacket parseLight(byte* data){
    return *((lightPacket *) data);
}

void serverSendLight(byte* address, lightPacket lght){
    Serial.print("{\"light\"{ ");
    Serial.print("\"address\": [");
    Serial.print(address[0]);Serial.print(",");
    Serial.print(address[1]);Serial.print(",");
    Serial.print(address[2]);Serial.print(",");
    Serial.print(address[3]);
    Serial.print("], \"timestamp\": ");
    Serial.print(lght.timestamp);
    Serial.print(", \"light\": ");
    Serial.print(lght.light);
    Serial.println(" } }");
}
