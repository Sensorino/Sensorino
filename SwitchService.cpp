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


switchPacket serverParseSwitch(char* line, byte* address){
    switchPacket pkt;
    pkt.status = false;
    if(line != NULL){
        pkt.status = JSONtoBoolean(line, "status");
        char* addrstrs[4];
        int len=0;
        char* pt = JSONsearchDataName(line, "address");
        JSONtoStringArray(line, addrstrs, &len);
        if(len == 4){
            address[0] = (byte) strtoul(addrstrs[0], NULL, 10);
            address[1] = (byte) strtoul(addrstrs[1], NULL, 10);
            address[2] = (byte) strtoul(addrstrs[2], NULL, 10);
            address[3] = (byte) strtoul(addrstrs[3], NULL, 10);
        }
    }
    return pkt;
}
