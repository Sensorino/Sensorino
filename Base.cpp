/** Library that implements all the functionalities of the Base node.
 * It imports the functionalities from Sensorino_JSON for managing messaging with the base
 * and those included in Sensorino_Protocol for connectivity
 * plus, it adds the support for BaseService objects
 *
 * Author: Dario Salvi (dariosalvi78 at gmail dot com)
 *
 * Licensed under the GPL license http://www.gnu.org/copyleft/gpl.html
 */
#ifdef __cplusplus
extern "C"
#endif

#include <Base.h>


static BaseService* services[MAX_SERVICES];
static unsigned int servicesN;


static void handleJSONServiceMessages(MessageType msgtype, byte* address,unsigned int serviceID, byte servInstID, char* message){
    if((msgtype == PUBLISH) || (msgtype == SET) || (msgtype == REQUEST)){
        for(int i=0; i<servicesN; i++){
            if(services[i]->getServiceTypeID() == serviceID){
                services[i]->handleJSONMessage(msgtype, address, servInstID, message);
            }
        }
    }
}

boolean addService(BaseService* serv){
    if(servicesN >= MAX_SERVICES) return false;
    services[servicesN] = serv;
    servicesN ++;
}

boolean initServices(){
    setJSONServiceMessageHandler(handleJSONServiceMessages);

    boolean OK = true;
    for(int i=0; i<servicesN; i++)
        if(! services[i]->init()) OK = false;
    return OK;
}

boolean runServices(){
    boolean OK = true;
    for(int i=0; i<servicesN; i++){
        if(!services[i]->run()) OK = false;
    }
    return OK;
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
                if(services[i]->getServiceTypeID() == serviceID){
                       services[i]->handleMessage(broadcast, sender, msgType, serviceInstID, df, serviceData, datalen);
                   }
            }
        }
}
