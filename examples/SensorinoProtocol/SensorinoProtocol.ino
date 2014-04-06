/** An example on how to use Sensorino protocol.
 * You need a sensorino node and a base.
 * The sensorino node sends 3 messages, one control, one error and one service
 * The base receives and parses the three messages
 */
#include <SPI.h>
#include <nRF24.h>
#include <Sensorino_Protocol.h>


//Comment this line to act as a sensorino
//#define ROLE_BASE

void setup() {
  Serial.begin(57600);
  Serial.print("Sensorino protocol example, role: ");

#ifdef ROLE_BASE
  Serial.println("base");
  if(!startRadio(9, 10, 2, baseAddress)) Serial.println("Cannot init!");
#else
  Serial.println("sensorino");
  byte myAddr[4] = {1,100,2,200}; //Sensorino address
  if(!startRadio(9, 10, 2, myAddr)) Serial.println("Cannot init!");
#endif
}

void loop() {
#ifdef ROLE_BASE
    boolean broadcast = false;
    byte sender[4];
    MessageType msgType;
    byte databuffer[MAX_SENSORINO_PAYLOAD_LEN];
    int len;
    if(receive(5000, &broadcast, sender, &msgType, databuffer, &len)){
        Serial.print("Got message from ");
        Serial.print(sender[0]);Serial.print(".");
        Serial.print(sender[1]);Serial.print(".");
        Serial.print(sender[2]);Serial.print(".");
        Serial.print(sender[3]);
        Serial.print(" type ");Serial.print(msgType);
        if(msgType == CTRL){
            ControlType ctrlt;
            byte ctrlData[MAX_CONTROL_PAYLOAD_LEN];
            int datalen;
            unmakeCtrlPayload(databuffer, len, &ctrlt, ctrlData, &datalen);
            Serial.print(" control type "); Serial.print(ctrlt);
            Serial.print(" data ");
            for(int i=0; i<datalen; i++)
                Serial.print(ctrlData[i]);
            Serial.println();
        }
        if(msgType == ERR){
            ErrorType errt;
            byte errData[MAX_ERROR_PAYLOAD_LEN];
            int datalen;
            unmakeErrorPayload(databuffer, len, &errt, errData, &datalen);
            Serial.print(" error type "); Serial.print(errt);
            Serial.print(" data ");
            for(int i=0; i<datalen; i++)
                Serial.print(errData[i]);
            Serial.println();
        }
        if((msgType == PUBLISH)||(msgType == REQUEST)||(msgType == SET)){
            unsigned int serviceID;
            byte serviceInstID;
            DataFormat df;
            byte serviceData[MAX_SERVICE_PAYLOAD_LEN];
            int datalen;
            unmakeServicePayload(databuffer, len, &serviceID, &serviceInstID, &df, serviceData, &datalen);
            Serial.print(" service id "); Serial.print(serviceID);
            Serial.print(" service instance id "); Serial.print(serviceInstID);
            Serial.print(" data format "); Serial.print(df);
            Serial.print(" data ");
            for(int i=0; i<datalen; i++)
                Serial.print(serviceData[i]);
            Serial.println();
        }
    }
#else
    Serial.println("Sending example messages");
    if(!sendControl(false, baseAddress, PING, (byte*)"hello", 6)) Serial.println("Cannot send ping");
    delay(2000);
    if(!sendError(false, baseAddress, SERVICE_UNAVAILABLE, (byte*)"how are you", 12)) Serial.println("Cannot send error");
    delay(2000);
    if(!sendService(false, baseAddress, 100, 1, PUBLISH, ADHOC, (byte*)"fine thanks", 12)) Serial.println("Cannot send error");
    delay(2000);
#endif
}
