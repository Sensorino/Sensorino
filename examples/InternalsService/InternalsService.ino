/** Tests the Internals service.
 * You need a sensorino node and a base.
 * The base receives the internals messages
 * and sends the mesage to the serial link
 * The sensorino node sends its internals.
 */
#include <SPI.h>
#include <nRF24.h>
#include <Sensorino.h>
#include <Base.h>
#include <InternalsService.h>

//Comment this line to act as a sensorino
//#define ROLE_BASE

void setup() {
  Serial.begin(57600);
  Serial.print("Internals service, role: ");
#ifdef ROLE_BASE
  Serial.println("base");
  configureBase(9, 10, 2);
  if(!startBase()) Serial.println("Cannot init!");;
#else
  Serial.println("sensorino");
  byte myAddr[4] = {1,100,2,200}; //Sensorino address
  configure(9, 10, 2, myAddr);
  if(!start()) Serial.println("Cannot init!");
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
    if((!broadcast) && (serv == INTERNALS_SERVICE)){
      Serial.println("Got internals: ");
      internalsPacket p =  parseInternals(data);
      serverSendInternals(sender, p);
     }
  }
#else
  Serial.println("Sending internals");
  internalsPacket pkt;
  pkt.timestamp = 123456;
  pkt.vcc = readVcc();
  pkt.temperature = readTemp();
  if(!sendInternals(pkt)) Serial.println("Cannot send internals");
  sleep();
#endif
}
