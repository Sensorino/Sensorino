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
unsigned long lastPeriodicCounter = 0;

unsigned long getTime(){
    if(lastUnixTime != 0){
        unsigned long time = lastUnixTime + ( (millis() - lastTimeStamp)/1000 );
        if(periodicWakeUpCounter != -1){
            time += (periodicWakeUpCounter - lastPeriodicCounter) * 8;
            lastPeriodicCounter = periodicWakeUpCounter;
        }
        setTime(time);
    }
    return lastUnixTime;
}

void setTime(unsigned long ts){
    lastTimeStamp = millis();
    lastUnixTime = ts;
}

boolean askTime(){
    byte message[4] = {0,0,0,0}; //zeros message = "ask for time"
    if(!sendToBase(TIME_SERVICE, message, 4))
        return false;
    unsigned int receivedService = 0;
    byte receivedMessage[30];
    byte sender[4];
    int receivedLen = 0;
    boolean broadcast;
    if(receive(2000, &broadcast, sender, &receivedService, receivedMessage, &receivedLen)) {
        if((!broadcast) && (receivedService == TIME_SERVICE) && (receivedLen>=sizeof(timePacket))){
            timePacket pkt = *((timePacket *) receivedMessage);
            setTime( pkt.timestamp );
            return true;
        }
    }
    return false;
}

boolean serveTime(byte* address){
    unsigned long t = getTime();
    if(t != 0){
        timePacket pkt;
        pkt.timestamp =t;
        return sendToSensorino(address, TIME_SERVICE, (byte *) &pkt, sizeof(pkt));
    }
    return false;
}

boolean askServerTime(){
    Serial.println("{ \"command\": \"getTime\" }");
    char buff[100];
    int chars = Serial.readBytesUntil('\n', buff, 100);
    if(chars >=21){
        //Expected format { "time": 1391796357 }
        unsigned long ts = JSONtoULong(buff, "\"time\":");
        setTime(ts);
        if(ts != 0)
            return true;
    }
    return false;
}
