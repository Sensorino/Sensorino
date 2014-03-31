/** An example about Sensorino JSON parsing.
 * Reads the serial port until a message is found.
 * Try with:
 * { "control": { "address": [1,2,3,4], "type": "PING", "data": { "text": "test" } } }
 * { "error": { "address": [1,2,3,4], "type": "SERVICE_UNAVAILABLE", "data": { "text": "test" } } }
 * { "set": { "address": [1,2,3,4], "serviceID": 10, "serviceInstanceID": 0, "data": { "text":"test" } } }
 * { "request": { "address": [1,2,3,4], "serviceID": 10, "serviceInstanceID": 0, "data": { "text":"test" } } }
 */
#include <SPI.h>
#include <nRF24.h>
#include <Sensorino_JSON.h>

void handlePing(byte* address, char* data){
    Serial.print("Got a ping message ! address: ");
    Serial.print(address[0]);Serial.print(",");
    Serial.print(address[1]);Serial.print(",");
    Serial.print(address[2]);Serial.print(",");
    Serial.print(address[3]);
    if(data != NULL){
        Serial.print(" data: ");
        Serial.println(data);
    }
    Serial.print("...replying like this: ");
    char buffer[200];
    makeJSONControl(buffer, PONG, address, data);
    Serial.print(buffer);
}


void handleSUError(byte* address, char* data){
    Serial.print("Got an error message ! address: ");
    Serial.print(address[0]);Serial.print(",");
    Serial.print(address[1]);Serial.print(",");
    Serial.print(address[2]);Serial.print(",");
    Serial.print(address[3]);
    if(data != NULL){
        Serial.print(" data: ");
        Serial.println(data);
    }
    Serial.print("...replying like this: ");
    char buffer[200];
    makeJSONError(buffer, SERVICE_UNAVAILABLE, address, data);
    Serial.print(buffer);
}

void handleService10Handler(MessageType msgt, byte* address, byte servInst, char* data){
    Serial.print("Got a service message ! type: ");
    Serial.print(msgt);
    Serial.print(" address: ");
    Serial.print(address[0]);Serial.print(",");
    Serial.print(address[1]);Serial.print(",");
    Serial.print(address[2]);Serial.print(",");
    Serial.print(address[3]);
    Serial.print(" instance: ");
    Serial.print(servInst);
    if(data != NULL){
        Serial.print(" data: ");
        Serial.println(data);
    }
    Serial.print("...replying like this: ");
    char buffer[200];
    makeJSONService(buffer, PUBLISH, address, 10, 0, data);
    Serial.print(buffer);
}

void setup() {
  Serial.begin(57600);
  Serial.println("Sensorino JSON parsing example");
  if(!addJSONControlMessageHandler(PING, handlePing)) Serial.println("cannot add handler");
  if(!addJSONErrorMessageHandler(SERVICE_UNAVAILABLE, handleSUError)) Serial.println("cannot add handler");
  if(!addJSONServiceMessageHandler(10, handleService10Handler)) Serial.println("cannot add handler");
}

void loop() {
    readSerial(2000);
}

