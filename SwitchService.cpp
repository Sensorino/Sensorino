/** Switch service: a service for sending information about a on/off switch.
 * This service works on the unicast channel (pipe 1).
 * Two data are supported:
 * timestamp: the time of the node
 * status: on or off
 *
 * Author: Dario Salvi (dariosalvi78 at gmail dot com)
 *
 * Licensed under the GPL license http://www.gnu.org/copyleft/gpl.html
 */
#ifdef __cplusplus
extern "C"
#endif
#include "SwitchService.h"

boolean sendSwitch(switchPacket pkt){
    return sendToBase(SWITCH_SERVICE, (byte*)&pkt, sizeof(switchPacket));
}


switchPacket parseSwitch(byte* data){
    return *((switchPacket *) data);
}


void serverSendSwitch(byte* address, switchPacket sw){
    Serial.print("{\"switch\"{ ");
    Serial.print("\"address\": [");
    Serial.print(address[0]);Serial.print(",");
    Serial.print(address[1]);Serial.print(",");
    Serial.print(address[2]);Serial.print(",");
    Serial.print(address[3]);
    Serial.print("], \"timestamp\": ");
    Serial.print(sw.timestamp);
    Serial.print(", \"status\": ");
    Serial.print(sw.status >0? "true":"false");
    Serial.println(" } }");
}
