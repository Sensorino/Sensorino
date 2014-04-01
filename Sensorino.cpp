/** Library that implements all the functionalities of a Sensorino node.
 * It imports the functionalities from Sensorino_Energy for energy management
 * and those included in Sensorino_Protocol for connectivity
 * plus, it adds the support for SensorinoService objects
 *
 * Author: Dario Salvi (dariosalvi78 at gmail dot com)
 *
 * Licensed under the GPL license http://www.gnu.org/copyleft/gpl.html
 */
#include <Sensorino.h>

#define MAX_PINS MAX_WAKEUP_PINS * MAX_SERVICES


//Euclidean algorithm
static int gcd(int a, int b){
    if(b == 0)
       return a;
    else
       return gcd(b, a % b);
}

int sleepSecs = -1;

SensorinoService* services[MAX_SERVICES];
unsigned int servicesN = 0;
unsigned long lastExecution[MAX_SERVICES];
int pins[MAX_PINS];
unsigned int pinsN = 0;

boolean addService(SensorinoService* serv){
    if(servicesN >= MAX_SERVICES) return false;
    services[servicesN] = serv;
    servicesN++;
    //Compute sleep time
    int srvslpt = serv->getSleepPeriod();
    if((sleepSecs<0) && (srvslpt >=0))
        sleepSecs = 0;
    if(srvslpt>=0)
        sleepSecs = gcd(sleepSecs, srvslpt);
    //Accumulate pins
    for(int i=0; i<serv->getWakeupPinsNumber(); i++){
        pins[pinsN] = serv->getWakeupPins()[i];
        pinsN ++;
        if(pinsN >= MAX_PINS) return false;
    }
    return true;
}


static unsigned long getSecs(){
    return getTotalSleepSeconds() + (millis() / 1000);
}

boolean initServices(){
    for(int i=0; i<servicesN; i++)
        lastExecution[i] = 0;

    boolean OK = true;
    for(int i=0; i<servicesN; i++)
        if(! services[i]->init()) OK = false;
    return OK;
}


boolean runServices(){
    boolean OK = true;
    for(int i=0; i<servicesN; i++){
        int sleepPeriod = services[i]->getSleepPeriod();
        if(sleepPeriod <=0){ //services with sleep period as 0 or <0 are always served
            if(!services[i]->run()) OK = false;
        }
        else if((getSecs() - lastExecution[i]) >= sleepPeriod){
            lastExecution[i] = getSecs();
            if(!services[i]->run()) OK = false;
        }

    }
    return OK;
}

void sleep(){
    sleepUntil(sleepSecs, pins, pinsN);
}


boolean receive(unsigned int timeoutMS){
    boolean broadcast;
    byte sender[4];
    MessageType msgType;
    byte databuffer[MAX_SENSORINO_PAYLOAD_LEN];
    int len;
    boolean received = receive(timeoutMS, &broadcast, sender, &msgType, databuffer, &len);
    if(!received) return false;

    if(msgType == CTRL){
            ControlType ctrlt;
            byte ctrlData[MAX_CONTROL_PAYLOAD_LEN];
            int datalen;
            unmakeCtrlPayload(databuffer, len, &ctrlt, ctrlData, &datalen);
            //TODO: serve control packet
        }
        if(msgType == ERR){
            ErrorType errt;
            byte errData[MAX_ERROR_PAYLOAD_LEN];
            int datalen;
            unmakeErrorPayload(databuffer, len, &errt, errData, &datalen);
            //TODO: serve control packet
        }
        if((msgType == PUBLISH)||(msgType == REQUEST)||(msgType == SET)){
            unsigned int serviceID;
            byte serviceInstID;
            DataFormat df;
            byte serviceData[MAX_SERVICE_PAYLOAD_LEN];
            int datalen;
            unmakeServicePayload(databuffer, len, &serviceID, &serviceInstID, &df, serviceData, &datalen);
            for(int i=0; i<servicesN; i++){
                if((services[i]->getServiceTypeID() == serviceID) &&
                   (services[i]->getServiceInstanceID() == serviceInstID)){
                       services[i]->handleMessage(broadcast, msgType, df, serviceData, datalen);
                   }
            }
        }
}
