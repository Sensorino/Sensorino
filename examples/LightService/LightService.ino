/** An example of a light sensor.
 * You need a Sensorino node and a base.
 * The Sensorino must have a LDR connected to the analog pin 0.
 * In this configuration we use the internal pullup resistor to 
 * avoid having an external one and to avoid activating the LDR
 * with some external transistor.
 * Electric configuration: just put the LDR between analog pin 0
 * and ground.
 * The base receives the messages and passes them to the computer.
 */
#include <SPI.h>
#include <nRF24.h>
#include <Sensorino.h>
#include <Base.h>
#include <LightService.h>

//Comment this line to act as a Sensorino
#define ROLE_BASE

const int LDRpin= A0; //change this if you want to use another pin

void setup() {
  Serial.begin(57600);
  Serial.print("Light service, role: ");
#ifdef ROLE_BASE
  Serial.println("base");
  configureBase(9, 10, 2);
  if(!startBase()) Serial.println("Cannot init!");;
#else
  Serial.println("sensorino");
  byte myAddr[4] = {1,100,2,200}; //Sensorino address
  configure(9, 10, 2, myAddr);
  if(!start()) Serial.println("Cannot init!");
  wakeUpPeriodically(1); //wakes up every 8 seconds
#endif
}

void loop() {
#ifdef ROLE_BASE
  boolean broadcast = false;
  word serv = 0;
  byte sender[4];
  byte data[30];
  int len = 0;
  Serial.println("...waiting for incoming packet....");
  if(receive((unsigned int)5000, &broadcast, sender, &serv, data, &len)){
    if((!broadcast) && (serv == LIGHT_SERVICE)){
      Serial.print("Got light: ");
      lightPacket p =  parseLight(data);
      serverSendLight(sender, p);
     }
  }
#else
  Serial.println("Sending light information");
  lightPacket pkt;
  pkt.timestamp = 0; //to set a proper timestamp, use the TimeService
  pinMode(LDRpin, INPUT_PULLUP); //activate the internal pullup
  delay(500); //wait a little bit to let the LDR react
  int val = analogRead(LDRpin);
  pinMode(LDRpin, INPUT);//now we can switch off the LDR
  pkt.light = 1023 - val; //using a pull-up the logic is inverted
  if(!sendLight(pkt)) Serial.println("Cannot send light information");
  sleep();
#endif
}
