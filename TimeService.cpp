/** Time service: a service for synchronizing clocks.
 * This service works on the broadcast channel  (pipe 0).
 * Every isntance can work as a client or server.
 * A messagge is sent by the node that wants to set the time.
 * All nodes that have the time set should answer.
 */
#ifdef __cplusplus
extern "C"
#endif
#include "Sensorino.h" //needed to have reference to the periodicWakeUpCounter
#include "TimeService.h"

unsigned long lastUnixTime = 0;
unsigned long lastTimeStamp = 0;

unsigned long getTime(){
    if(lastUnixTime != 0){
        unsigned long time = lastUnixTime + ( (millis() - lastTimeStamp)/1000 );
        if(periodicWakeUpCounter != -1){
            time += periodicWakeUpCounter * 8;
            periodicWakeUpCounter = 0;
        }
        return time;
    }
    else return 0;
}

void setTime(unsigned long ts){
    lastTimeStamp = millis();
    lastUnixTime = ts;
}

void askTime(){
    byte message[4] = {0,0,0,0}; //zeros message = "ask for time"
    if(!sendToBroadcast(TIME_SERVICE, message, 4))
        return;
    unsigned int receivedService = 0;
    byte receivedMessage[30];
    byte sender[4];
    int receivedLen = 0;
    boolean broadcast;
    if(receive(2000, &broadcast, sender, &receivedService, receivedMessage, &receivedLen)) {
        if((broadcast) && (receivedService == TIME_SERVICE) && (receivedLen>=sizeof(timePacket))){
            timePacket pkt = *((timePacket *) receivedMessage);
            setTime( pkt.timestamp );
        }
    }
}

void serveTime(){
    unsigned long t = getTime();
    if(t != 0){
        timePacket pkt;
        pkt.timestamp =t;
        sendToBroadcast(TIME_SERVICE, (byte *) &pkt, sizeof(pkt));
    }
}

void askServerTime(){
    Serial.println("{ \"command\" : \"getTime\" }");
    char buff[100];
    String timestring = readLineFromSerial(buff);
    if(timestring != NULL)
    parseServerTime(timestring);
}

void parseServerTime(String line){
    //Expected format { "time" : 1391796357 }
    if(line.length() >= 21){
        String number = line.substring(11);
        int len = number.length();
        char buff[len+1];
        number.toCharArray(buff, len);
        buff[len]='\0';
        unsigned long ts = strtoul(buff, NULL, 10);
        setTime(ts);
    }
}
