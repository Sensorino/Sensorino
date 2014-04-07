/** Trivial JSON messages parser for Sensorino base.
 * messages follow a known structure depending on the type of message:
 * { "publish": { "address": [1,2,3,4], "serviceID": 2, "serviceInstanceID": 0, "data": { ..... } } }
 * { "set": { "address": [1,2,3,4], "serviceID": 2, "serviceInstanceID": 0, "data": { ..... } } }
 * { "request": { "address": [1,2,3,4], "serviceID": 2, "serviceInstanceID": 0, "data": { ..... } } }
 * { "control": { "address": [1,2,3,4], "type": "ADVERT", "data": { ..... } }
 * { "control": { "address": [1,2,3,4], "type": "PING", "data": { ..... } } }
 * { "control": { "address": [1,2,3,4], "type": "PONG", "data": { ..... } } }
 * { "error": { "address": [1,2,3,4], "type": "SERVICE_UNAVAILABLE", "data": { ..... } } }
 *
 * Author: Dario Salvi (dariosalvi78 at gmail dot com)
 *
 * Licensed under the GPL license http://www.gnu.org/copyleft/gpl.html
 */
#ifdef __cplusplus
extern "C"
#endif

#include <Sensorino_JSON.h>

#define JSON_STRING_BUFFER_LEN 150
#define MAX_JSON_CTRL_HANDLERS_LEN 5
#define MAX_JSON_ERR_HANDLERS_LEN 5
#define MAX_JSON_SERV_HANDLERS_LEN 10

void JSONtoStringArray(char* line, char** arr, int* len) {
    *len = 0;
    if(line == NULL) return;
    char* substrstart = line;
    int level = 0;
    int arridx = 0;
    for(int i=0; i<strlen(line);i++){
        char* ptr = line +i;
        if(ptr == NULL) return;
        if(*ptr == '[') {
            if(level == 0){
                arr[arridx] = ptr+1;
                arridx++;
            }
            level ++;
        }
        if((*ptr == ',') && (level == 1)){
             arr[arridx] = ptr+1;
             arridx++;
        }
        if(*ptr == ']'){
            if(level == 0) break;
            level --;
        }
    }
    *len = arridx;
}

char* JSONsearchDataName(char* line, char* dataname){
    char* dataptr = strstr(line, dataname);
    char* ptr = dataptr + strlen(dataname);
    for(int i=0; i<strlen(dataptr); i++){
        ptr += i;
        if(*ptr == ':'){
            return ptr+1;
        }
    }
    return NULL;
}

unsigned long JSONtoULong(char* line, char* dataName) {
  char* dataptr = JSONsearchDataName(line, dataName);
  if (dataptr != NULL) {
      return strtoul(dataptr, NULL, 10);
  } return 0;
}

double JSONtoDouble(char* line, char* dataName) {
  char* dataptr = JSONsearchDataName(line, dataName);
  if (dataptr != NULL) {
      return strtod(dataptr, NULL);
  } return 0;
}

boolean JSONtoBoolean(char* line, char* dataName) {
  char* dataptr = JSONsearchDataName(line, dataName);
  if (dataptr != NULL) {
        if((toupper(dataptr[0]) == 'T')&&
           (toupper(dataptr[1]) == 'R')&&
           (toupper(dataptr[2]) == 'U')&&
           (toupper(dataptr[3]) == 'E'))
           return true;
      else return false;
  } return false;
}


void messageTypeToString(char* buffer, MessageType msgtype){
switch(msgtype){
    case PUBLISH:
        strcpy(buffer, "\"publish\"");
        break;
    case REQUEST:
        strcpy(buffer, "\"request\"");
        break;
    case SET:
        strcpy(buffer, "\"set\"");
    break;
        case CTRL:
        strcpy(buffer, "\"control\"");
        break;
    case ERR:
        strcpy(buffer, "\"error\"");
        break;
    }
}


MessageType stringToMessageType(char* str){

    if((toupper(str[0]) == 'C') && (toupper(str[1]) == 'O') && (toupper(str[2]) == 'N') &&
       (toupper(str[3]) == 'T') && (toupper(str[4]) == 'R') && (toupper(str[5]) == 'O') &&
       (toupper(str[6]) == 'L')){
        return CTRL;
    }
    else if((toupper(str[0]) == 'E') && (toupper(str[1]) == 'R') && (toupper(str[2]) == 'R') &&
            (toupper(str[3]) == 'O') && (toupper(str[4]) == 'R')){
        return ERR;
    }
    else if((toupper(str[0]) == 'P') && (toupper(str[1]) == 'U') && (toupper(str[2]) == 'B') &&
            (toupper(str[3]) == 'L') && (toupper(str[4]) == 'I') && (toupper(str[5]) == 'S') &&
            (toupper(str[6]) == 'H')){
        return PUBLISH;
    }
    else if((toupper(str[0]) == 'S') && (toupper(str[1]) == 'E') && (toupper(str[2]) == 'T')){
        return SET;
    }
    else if((toupper(str[0]) == 'R') && (toupper(str[1]) == 'E') && (toupper(str[2]) == 'Q') &&
            (toupper(str[3]) == 'U') && (toupper(str[4]) == 'E') && (toupper(str[5]) == 'S') &&
            (toupper(str[6]) == 'T')){
        return REQUEST;
    }
}


void controlTypeToString(char* buffer, ControlType ctrltype){
switch(ctrltype){
    case PING:
        strcpy(buffer, "\"PING\"");
        break;
    case PONG:
        strcpy(buffer, "\"PONG\"");
        break;
    case ADVERT:
        strcpy(buffer, "\"ADVERT\"");
        break;
    case TIMESYNCH:
        strcpy(buffer, "\"TIMESYNCH\"");
        break;
    }
}

ControlType stringToControlType(char* str){
    if((toupper(str[0]) == 'P')&&(toupper(str[1]) == 'I')&&(toupper(str[2]) == 'N')&&(toupper(str[3]) == 'G')) return PING;
    else if((toupper(str[0]) == 'P')&&(toupper(str[1]) == 'O')&&(toupper(str[2]) == 'N')&&(toupper(str[3]) == 'G')) return PONG;
    else if((toupper(str[0]) == 'A')&&(toupper(str[1]) == 'D')&&(toupper(str[2]) == 'V')&&(toupper(str[3]) == 'E')&&
       (toupper(str[4]) == 'R')&&(toupper(str[5]) == 'T')) return ADVERT;
    else if((toupper(str[0]) == 'T')&&(toupper(str[1]) == 'I')&&(toupper(str[2]) == 'M')&&(toupper(str[3]) == 'E')&&
       (toupper(str[4]) == 'S')&&(toupper(str[5]) == 'Y')&&(toupper(str[6]) == 'N')&&(toupper(str[7]) == 'C')&&
       (toupper(str[8]) == 'H')) return TIMESYNCH;
}

void errorTypeToString(char* buffer, ErrorType errtype){
switch(errtype){
    case SERVICE_UNAVAILABLE:
        strcpy(buffer, "\"SERVICE_UNAVAILABLE\"");
        break;
    case DATA_FORMAT_UNSUPPORTED:
        strcpy(buffer, "\"DATA_FORMAT_UNSUPPORTED\"");
        break;
    case CANNOT_PARSE_DATA:
        strcpy(buffer, "\"CANNOT_PARSE_DATA\"");
        break;
    }
}


ErrorType stringToErrorType(char* str){
    if((toupper(str[0]) == 'S') && (toupper(str[1]) == 'E') && (toupper(str[2]) == 'R') &&
       (toupper(str[3]) == 'V') && (toupper(str[4]) == 'I') && (toupper(str[5]) == 'C') &&
       (toupper(str[6]) == 'E') && (toupper(str[7]) == '_')) return SERVICE_UNAVAILABLE;
    else if((toupper(str[0]) == 'D') && (toupper(str[1]) == 'A') && (toupper(str[2]) == 'T')&&
            (toupper(str[3]) == 'A') && (toupper(str[4]) == '_')) return DATA_FORMAT_UNSUPPORTED;
    else if((toupper(str[0]) == 'C') && (toupper(str[1]) == 'A') && (toupper(str[2]) == 'N ')&&
            (toupper(str[3]) == 'N') && (toupper(str[4]) == 'O') && (toupper(str[5]) == 'T') &&
            (toupper(str[6]) == '_')) return CANNOT_PARSE_DATA;
}

int makeJSONControl(char* buffer, ControlType ctrltype, byte* address, char* data){
    strcpy(buffer, "{ \"control\": { \"address\": [");
    char num[4];
    strcat(buffer, itoa(address[0], num, 10)); strcat(buffer, ",");
    strcat(buffer, itoa(address[1], num, 10)); strcat(buffer, ",");
    strcat(buffer, itoa(address[2], num, 10)); strcat(buffer, ",");
    strcat(buffer, itoa(address[3], num, 10));
    strcat(buffer, "], \"type\": ");
    char type[50];
    controlTypeToString(type, ctrltype);
    strcat(buffer, type);
    if(data != NULL){
        strcat(buffer, ", \"data\":");
        strcat(buffer, data);
    }
    strcat(buffer, " } }\n");
    return strlen(buffer);
}

int makeJSONError(char* buffer, ErrorType errtype, byte* address, char* data){
    strcpy(buffer, "{ \"error\": { \"address\": [");
    char num[4];
    strcat(buffer, itoa(address[0], num, 10)); strcat(buffer, ",");
    strcat(buffer, itoa(address[1], num, 10)); strcat(buffer, ",");
    strcat(buffer, itoa(address[2], num, 10)); strcat(buffer, ",");
    strcat(buffer, itoa(address[3], num, 10));
    strcat(buffer, "], \"type\": ");
    char type[50];
    errorTypeToString(type, errtype);
    strcat(buffer, type);
    if(data != NULL){
        strcat(buffer, ", \"data\":");
        strcat(buffer, data);
    }
    strcat(buffer, " } }\n");
    return strlen(buffer);
}

int makeJSONService(char* buffer, MessageType msgtype, byte* address, unsigned int serviceID, byte serviceInstID, char* data){
    strcpy(buffer, "{ ");
    char msgtypestr[10];
    messageTypeToString(msgtypestr, msgtype);
    strcat(buffer, msgtypestr);
    strcat(buffer, ": { \"address\": [");
    char num[10];
    strcat(buffer, itoa(address[0], num, 10)); strcat(buffer, ",");
    strcat(buffer, itoa(address[1], num, 10)); strcat(buffer, ",");
    strcat(buffer, itoa(address[2], num, 10)); strcat(buffer, ",");
    strcat(buffer, itoa(address[3], num, 10));
    strcat(buffer, "], \"serviceID\": ");
    strcat(buffer, itoa(serviceID, num, 10));
    strcat(buffer, ", \"serviceInstanceID\": ");
    strcat(buffer, itoa(serviceInstID, num, 10));
    if(data != NULL){
        strcat(buffer, ", \"data\":");
        strcat(buffer, data);
    }
    strcat(buffer, " } }\n");
    return strlen(buffer);
}


void (*JSONCtrlhandler)(ControlType ctrltype, byte* address, char* message);

void setJSONControlMessageHandler(void (*h)(ControlType ctrltype, byte* address, char* message)){
    JSONCtrlhandler = h;
}

void (*JSONErrhandler)(ErrorType errtype, byte* address, char* message);

void setJSONErrorMessageHandler(void (*h)(ErrorType errtype, byte* address, char* message)){
    JSONErrhandler = h;
}

void (*JSONServhandler)(MessageType msgtype, byte* address, unsigned int serviceID, byte servInstID, char* message);

void setJSONServiceMessageHandler(void (*h)(MessageType msgtype, byte* address, unsigned int serviceID, byte servInstID, char* message)){
    JSONServhandler = h;
}


static void parseAddress(byte* address, char* msg){
    char* addrpt = JSONsearchDataName(msg, "address");
    int len = 0;
    char* addrstrs[4];
    JSONtoStringArray(addrpt, addrstrs, &len);
    if(len == 4){
        address[0] = (byte) strtoul(addrstrs[0], NULL, 10);
        address[1] = (byte) strtoul(addrstrs[1], NULL, 10);
        address[2] = (byte) strtoul(addrstrs[2], NULL, 10);
        address[3] = (byte) strtoul(addrstrs[3], NULL, 10);
    }
}

static int parseData(char* buff, char* msg){
    char* dataptr = JSONsearchDataName(msg, "data");
    if((*dataptr != NULL) && (*dataptr == '{')){ //there are some data indeed
    //already in the object
    int lvl = 1;
    int ind = 1;
    while(ind < strlen(dataptr)){
        if(dataptr[ind] == '{') lvl ++;
        if(dataptr[ind] == '}') lvl --;
        if (lvl ==0){ //now we copy the string
            for(int i=0; i<=ind; i++)
                buff[i] = dataptr[i];
            buff[ind+1] = '\0';
            return ind+1;
            }
        ind++;
        }
    }
    return 0;
}

static char buffer[JSON_STRING_BUFFER_LEN];
static int buffPtr = 0;
static int level =0;
static boolean inQuotes = false;

static char firstWordBuff[20];
static int firstWordBuffPtr = 0;
static boolean inFirstWord = false;

void readSerial(int mis){
    long now=  millis();

    while ((millis()-now < mis) || (Serial.available() >0)) {
        if(!Serial.available()) continue;
        char b = Serial.read();
        if(b=='"'){
            if(inQuotes){ //not in quote anymore
                inQuotes = false;
                inFirstWord = false;
            } else { //in quote
                inQuotes = true;
                if(firstWordBuffPtr == 0){
                    inFirstWord = true;
                }
            }
        }
        if(b=='{'){
            level++;
        }

        if((level>0) && ((inQuotes) || ((b!=' ')&&(b!='\n')&&(b!='\r')&&(b!='\t')))){
            buffer[buffPtr] = b;
            buffPtr++;
            if(inFirstWord){ //Fill the first word buffer
                firstWordBuff[firstWordBuffPtr] = b;
                firstWordBuffPtr++;
            }
        }

        if(b=='}'){
            if(level == 1){
                //The message is complete, use it
                char msg[buffPtr+1];
                for(int i=0; i<buffPtr; i++)
                    msg[i] = buffer[i];
                msg[buffPtr] = '\0';

                char firstword[firstWordBuffPtr];
                for(int i=0; i<firstWordBuffPtr-1; i++)
                    firstword[i] = firstWordBuff[i+1];
                firstword[firstWordBuffPtr-1] = '\0';

                //reset buffers
                buffPtr = 0;
                firstWordBuffPtr = 0;

                MessageType msgtype = stringToMessageType(firstword);
                //{ "control": { "address": [1,2,3,4], "type": "ADVERT", "data": { ..... } }
                if(msgtype == CTRL){
                    ControlType ctrltype = stringToControlType((JSONsearchDataName(msg, "type")+1));
                    byte address[4];
                    parseAddress(address, msg);
                    char databff[100];
                    char* datastr = NULL;
                    int len = parseData(databff, msg);
                    if(len != 0){
                        datastr = new char[len];
                        strcpy(datastr, databff);
                    }
                    //call handler
                    JSONCtrlhandler(ctrltype, address, datastr);
                }
                //{ "error": { "address": [1,2,3,4], "type": "SERVICE_UNAVAILABLE", "data": { "text": "test" } } }
                else if(msgtype == ERR){
                    ErrorType errtype = stringToErrorType((JSONsearchDataName(msg, "type")+1));
                    byte address[4];
                    parseAddress(address, msg);
                    char databff[100];
                    char* datastr = NULL;
                    int len = parseData(databff, msg);
                    if(len != 0){
                        datastr = new char[len];
                        strcpy(datastr, databff);
                    }
                    //call handler
                    JSONErrhandler(errtype,address, datastr);
                }
                //{ "publish": { "address": [1,2,3,4], "serviceID": 2, "serviceInstanceID": 0, "data": { ..... } } }
                //{ "set": { "address": [1,2,3,4], "serviceID": 2, "serviceInstanceID": 0, "data": { ..... } } }
                //{ "request": { "address": [1,2,3,4], "serviceID": 2, "serviceInstanceID": 0, "data": { ..... } } }
                else{
                    unsigned int servID = JSONtoULong(msg, "serviceID");
                    byte servInstID = JSONtoULong(msg, "serviceInstanceID");
                    byte address[4];
                    parseAddress(address, msg);
                    char databff[100];
                    char* datastr = NULL;
                    int len = parseData(databff, msg);
                    if(len != 0){
                        datastr = new char[len];
                        strcpy(datastr, databff);
                    }
                    //call handlers
                    JSONServhandler(msgtype, address, servID, servInstID, datastr);
                }
            }
            level--;
        }
    }
}


