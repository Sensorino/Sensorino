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
  askServerTime();
#else
  Serial.println("sensorino");
  byte myAddr[4] = {1,100,2,200};
  configure(9, 10, 2, myAddr);
  if(!start()) Serial.println("Cannot start!");
#endif
}

void loop() {
#ifdef ROLE_BASE
  boolean broadcast = true;
  word serv = 0;
  byte sender[4];
  byte data[30];
  int len = 0;
  Serial.println("...waiting for incoming packet....");
  if(receive((unsigned int)5000, &broadcast, sender, &serv, data, &len)){
    if((broadcast) && (serv == 1)){
      Serial.print("Serving time: ");
      Serial.println(getTime());
      serveTime();
     }
  }
#else
  Serial.print("Ask for time...");
  askTime();
  Serial.print("Current time: ");
  Serial.println(getTime());
  delay(5000);
#endif
}
