/** Example of usage of the time service.
 * The Base asks for the time on serial, waits until 5 seconds to get an answer
 * an answer can be given manually (example: { "time" : 1391796357 })
 * The Sensorino node asks for the time to the base periodically and sets it
 * when received.
 */
#include <SPI.h>
#include <nRF24.h>
#include <Sensorino.h>
#include <Base.h>
#include <TimeService.h>

//Comment this line to act as sensorino
//#define ROLE_BASE

void setup() {
  Serial.begin(57600);
  Serial.setTimeout(5000);//wait 5 seconds max to receive answers from serial
  Serial.print("Time service, role: ");
#ifdef ROLE_BASE
  Serial.println("base");
  configureBase(9, 10, 2);
  if(!startBase()) Serial.println("Cannot start!");
  if(!askServerTime()) Serial.println("Cannot receive time from computer");
#else
  Serial.println("sensorino");
  byte myAddr[4] = {1,100,2,200};
  configure(9, 10, 2, myAddr);
  if(!start()) Serial.println("Cannot start!");
  wakeUpPeriodically(1); //every 8 seconds
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
    if((!broadcast) && (serv == TIME_SERVICE)){
      Serial.print("Serving time: ");
      Serial.println(getTime());
      delay(250); //add some small delay for ensuring that the other side is listening
      if(!serveTime(sender)) Serial.println("Cannot serve time");
     }
  }
#else
  Serial.print("My time is: ");
  Serial.println(getTime());
  Serial.print("Asking for new time...");
  if(askTime()){
    Serial.print("Got time: ");
    Serial.println(getTime());
  } else {
    Serial.println("failed to get time");
  }
  Serial.print("Going to sleep in 5 seconds");
  delay(1000); Serial.print(".");
  delay(1000); Serial.print(".");
  delay(1000); Serial.print(".");
  delay(1000); Serial.print(".");
  delay(1000); Serial.println(".");
  sleep();
  Serial.println("Woken up!");
#endif
}
