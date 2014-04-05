/** An example about how to create a service for a Sensorino node.
 * The class TestService implements a service for Sensorino, the
 * corresponding example for the base shall be loaded into a Base.
 */
#include <SPI.h>
#include <nRF24.h>
#include <Sensorino.h>

const int serviceNumber  = 100;

//this implements a service for the Sensorino node (not the Base !!!)
class TestService: public SensorinoService{

public:
    //a data structure used to hold data treated by this service
    struct testData{
        boolean b;
        int i;
    };

    //constructor: it tells that the service number is 100, the instance is a parameter
    //it has to sleep for 5 seconds and is not supposed to be woken up by pins
    TestService(byte instance): SensorinoService(serviceNumber, instance, 5, 0){
    }

    //initializes the service at startup
    boolean init(){
        Serial.println("Test service started");
    }

    //when a packet is received for this service (and instance) it is passed to this function
    void handleMessage(boolean broadcast, MessageType msgType, DataFormat format, byte* data, int len){
        Serial.print("Test service: received packet");
    }

    //this is called periodically to run the service concretely, here goes the logic that
    //measures stuff, sends and receives data etc.
    boolean run(){
        Serial.println("Test service: sending a packet");
        testData data;
        data.b = true;
        data.i = 10;
        return sendService(false, baseAddress, serviceNumber, 0, ADHOC, (byte*) &data, sizeof(testData));
    }
};

//the instance of the service
TestService service(0);//0 is the isntance number

void setup() {
  Serial.begin(57600);
  Serial.println("Sensorino Test service example");

  //configure radio
  byte myAddr[4] = {1,100,2,200}; //Sensorino address
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
    receive(2000);

    //the sensorino goes to sleep
    Serial.println("..going to sleep");
    delay(100);
    sleep();
}

