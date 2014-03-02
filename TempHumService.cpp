/** Temperature and humidity service: a service for sending weather information.
 * This service works on the unicast channel (pipe 1).
 * Three data are supported now:
 * timestamp: the time of the node
 * temperature: environment temeprature in °„C x 10
 * humidity: the humidity in %
 *
 * Author: Dario Salvi (dariosalvi78 at gmail dot com)
 *
 * Licensed under the GPL license http://www.gnu.org/copyleft/gpl.html
 */
#ifdef __cplusplus
extern "C"
#endif
#include "TempHumService.h"

boolean sendTempHum(tempHumPacket pkt){
    return sendToBase(TEMPHUM_SERVICE, (byte*)&pkt, sizeof(tempHumPacket));
}


tempHumPacket parseTempHum(byte* data){
    return *((tempHumPacket *) data);
}


void serverSendTempHum(byte* address, tempHumPacket th){
    Serial.print("{\"temphum\"{ ");
    Serial.print("\"address\": [");
    Serial.print(address[0]);Serial.print(",");
    Serial.print(address[1]);Serial.print(",");
    Serial.print(address[2]);Serial.print(",");
    Serial.print(address[3]);
    Serial.print("], \"timestamp\": ");
    Serial.print(th.timestamp);
    Serial.print(", \"temperature\": ");
    Serial.print(((float)th.temperature)/10);
    Serial.print(", \"humidity\": ");
    Serial.print((th.humidity));
    Serial.println(" } }");
}
