#include "Services.h"

unsigned long Services::lastUnixTime = 0;
unsigned long Services::lastTimeStamp = 0;

unsigned long Services::getTime(){
    if((lastTimeStamp != 0) && (lastUnixTime != 0))
        return lastUnixTime + ( (millis() - lastTimeStamp)/1000 );
    else return 0;
}

void Services::setTime(unsigned long ts){
    lastTimeStamp = millis();
    lastUnixTime = ts;
}

void Services::askTime(){
    byte message[4] = {0,0,0,0}; //zeros message = "ask for time"
    sensorino.sendToBroadcast(TIME_SERVICE, message, 4);
    word receivedService = 0;
    byte receivedMessage[30];
    byte sender[4];
    int receivedLen = 0;
    byte pipe;
    sensorino.receive(2000, &pipe, sender,
                      &receivedService, receivedMessage, &receivedLen);
    if((pipe == BROADCAST_PIPE) && (receivedService == TIME_SERVICE) && (receivedLen>=4)){
        setTime( *((unsigned long *)message) );
    }
}

void Services::serveTime(byte * sender){
    unsigned long t = getTime();
    if(t != 0)
        sensorino.sendToBroadcast(TIME_SERVICE, (byte *)t, 4);
}
