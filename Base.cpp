/** Base, a base for sensorinos.
 * Adds communication to a server on the serial line.
 * Serial line messages are either debugging messages, either JSON messages.
 * JSON messages always start with { and end with }
 * In principle JSON messages are sent on one line.
 *
 * Author: Dario Salvi (dariosalvi78 at gmail dot com)
 *
 * Licensed under the GPL license http://www.gnu.org/copyleft/gpl.html
 */
#ifdef __cplusplus
extern "C"
#endif

#include <Base.h>

#define JSON_STRING_BUFFER_LEN 150
#define JSON_HANDLERS_LEN 5

void configureBase(byte chipEnablePin, byte chipSelectPin, byte irqPin) {
    nRF24.configure(chipEnablePin, chipSelectPin, irqPin);
}

boolean startBase(){
    //Init the nrf24
    nRF24.init();
    if(!nRF24.setChannel(RF_CHANNEL))return false;
    //set dynamic payload size
    if(!nRF24.setPayloadSize(0, 0))return false;
    if(!nRF24.setPayloadSize(1, 0))return false;
    //Set address size to 4
    if(!nRF24.setAddressSize(NRF24::NRF24AddressSize4Bytes))return false;
    //Set CRC to 2 bytes
    if(!nRF24.setCRC(NRF24::NRF24CRC2Bytes))return false;
    //Set 2 Mbps, maximum power
    if(!nRF24.setRF(NRF24::NRF24DataRate2Mbps, NRF24::NRF24TransmitPower0dBm))return false;
    //Configure pipes
    if(!nRF24.setPipeAddress(0, broadCastAddress))return false;
    if(!nRF24.enablePipe(0))return false;
    if(!nRF24.setAutoAck(0, false))return false;

    if(!nRF24.setPipeAddress(1, baseAddress))return false;
    if(!nRF24.enablePipe(1))return false;
    if(!nRF24.setAutoAck(1, true))return false;

    //Configure retries
    if(!nRF24.setTXRetries(3, 3))return false;

    //Starts listening
    if(!nRF24.powerUpRx())return false;

    return true;
}

boolean sendToSensorino(byte address[], word service, byte* data, int len){
    nRF24.setTransmitAddress(address);
    byte pkt[6+len];
    composeBasePacket(pkt, service, data, len);
    return nRF24.send(pkt, 6+len, false);
}

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

void (*JSONhandlers[JSON_HANDLERS_LEN])(char* msg);
char* words[JSON_HANDLERS_LEN];
int handlersPtr = 0;

boolean addJSONDataHandler(char* word, void (*h)(char* message)){
    if(handlersPtr == JSON_HANDLERS_LEN) return false;
    words[handlersPtr] = word;
    JSONhandlers[handlersPtr] = h;
    handlersPtr ++;
    return true;
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

                for(int i=0; i<JSON_HANDLERS_LEN; i++){
                    if(strcmp(firstword, words[i])==0){
                        JSONhandlers[i](msg);
                    }
                }
            }
            level--;
        }
    }
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
