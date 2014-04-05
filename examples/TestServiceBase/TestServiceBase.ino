/** An example about how to create a service for the Base node.
 * The class TestServiceBase implements a service for the Base, the
 * corresponding example for the Sensorino shall be loaded into a Sensorino.
 */
#include <SPI.h>
#include <nRF24.h>
#include <Base.h>

const int serviceNumber  = 100;

//this implements a service for the Base
class TestServiceBase: public BaseService {

public:
    //a data structure used to hold data treated by this service
    struct testData{
        boolean b;
        int i;
    };

    //constructor: it tells that the service number is 100
    TestServiceBase(): BaseService(serviceNumber){
    }

    //initializes the service at startup
    boolean init(){
        Serial.println("Test service started");
    }

    //when a packet is received for this service it is passed to this function
    void  handleMessage(boolean broadcast,byte* source, MessageType msgType, byte serviceInstanceID, DataFormat format, byte* data, int len){
        Serial.println("Test service received a packet");
        char buffer[200];

        testData td = *( (testData*) data);
        char dataBuffer[100];
        strcpy(dataBuffer, "{ \"b\": ");
        if(td.b) strcat(dataBuffer, "TRUE");
        else strcat(dataBuffer, "FALSE");
        strcat(dataBuffer, ", \"i\": ");
        char numberBuffer[10];
        strcat(dataBuffer, itoa(td.i, numberBuffer, 10));
        strcat(dataBuffer, " }");
        makeJSONService(buffer, msgType, source, serviceNumber, serviceInstanceID, dataBuffer);
    }

    //parses a JSON request for set
    //try sending { "set": { "address": [1,100,2,200], "serviceID": 100, "serviceInstanceID": 0, "data": { "b": TRUE, "i": 99 } } }
    void handleJSONMessage(MessageType msgtype, byte* address, byte servInstID, char* message){
        if(msgtype == SET){
            testData data;
            data.b = JSONtoBoolean("b", message);
            data.i = (int) JSONtoULong("i", message);
            sendService(false, address, serviceNumber, servInstID, ADHOC, (byte*) &data, sizeof(data));
        }
    }

    //this is called periodically to execute some logic that is initiated in the base
    boolean run(){
        //Nothing to do in this case
    }
};

//the instance of the service
TestServiceBase service;//0 is the isntance number

void setup() {
  Serial.begin(57600);
  Serial.println("Sensorino Test service example");

  //configure radio
  byte myAddr[4] = {BASE_ADDR}; //Base address
  if(!startRadio(9, 10, 2, myAddr)) Serial.println("Cannot init!");

  //add services
  addService(&service);

  //init services
  if(!initServices()) Serial.println("Cannot init services!");

}

void loop() {
    //run all services
    Serial.println("..going to run services");
    runServices();

    //receive packets if something is being waited
    Serial.println("..going to receive");
    receive(10000);
}

