/** RGB service: a service for controlling RGB lights and blinking.
 * This service works on the unicast channel (pipe 1).
 * The following data are supported:
 * red: the amount of red color (0 to 255)
 * green: the amount of green color (0 to 255)
 * blue: the amount of blue color (0 to 255)
 * blinkONtime = time in milliseconds the light must be on (0 to 65535)
 * blinkOFFtime = time in millisecond the light must be off (0 to 65535)
 *
 * Commands can be sent by the server following this format:
 * { "RGB"{ "address": [1,2,3,4], "red": 100, "green": 200 "blue": 0, "blinkONtime": 1000, "blinkOFFtime": 500 }}
 *
 * Author: Dario Salvi (dariosalvi78 at gmail dot com)
 *
 * Licensed under the GPL license http://www.gnu.org/copyleft/gpl.html
 */
#ifdef __cplusplus
extern "C"
#endif
#include "RGBService.h"


boolean sendRGBtoBase(RGBPacket pkt){
    return sendToBase(RGB_SERVICE, (byte*)&pkt, sizeof(RGBPacket));
}

boolean sendRGBtoSensorino(byte address[], RGBPacket pkt){
    return sendToSensorino(address, RGB_SERVICE,(byte*)&pkt, sizeof(RGBPacket));
}

RGBPacket parseRGB(byte* data){
    return *((RGBPacket *) data);
}

void serverSendRGB(byte* address, RGBPacket rgb){
    Serial.print("{\"RGB\"{ ");
    Serial.print("\"address\": [");
    Serial.print(address[0]);Serial.print(",");
    Serial.print(address[1]);Serial.print(",");
    Serial.print(address[2]);Serial.print(",");
    Serial.print(address[3]);
    Serial.print("], \"red\": ");
    Serial.print(rgb.red);
    Serial.print(", \"green\": ");
    Serial.print(rgb.green);
    Serial.print(", \"blue\": ");
    Serial.print(rgb.blue);
    Serial.print(", \"blinkONtime\": ");
    Serial.print(rgb.blinkONtime);
    Serial.print(", \"blinkOFFtime\": ");
    Serial.print(rgb.blinkOFFtime);
    Serial.println(" } }");
}

RGBPacket parseServerRGB(char* line, byte* address){
    //Expected format { "RGB"{ "address": [1,2,3,4], "red": 100, "green": 200 "blue": 0, "blinkONtime": 1000, "blinkOFFtime": 500 }}
    RGBPacket pkt;
    pkt.red = 0;
    pkt.green = 0;
    pkt.blue = 0;
    pkt.blinkONtime = 0;
    pkt.blinkOFFtime = 0;
    if(line != NULL){
        pkt.red = (byte) JSONtoULong(line, "\"red\":");
        pkt.green = (byte) JSONtoULong(line, "\"green\":");
        pkt.blue = (byte) JSONtoULong(line, "\"blue\":");
        pkt.blinkONtime = (byte) JSONtoULong(line, "\"blinkONtime\":");
        pkt.blinkOFFtime = (byte) JSONtoULong(line, "\"blinkOFFtime\":");
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
        return pkt;
    }
}

