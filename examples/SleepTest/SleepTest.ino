/** Tests the sleep mode.
 * There are two possibilitites to wake up Sensorino
 * periodically every 16 seconds or when a pin has changed state.
 * Don't forget to activate the wakeup or the MCU will never
 * recover from the sleep mode !
 */
#include <SPI.h>
#include <nRF24.h>
#include <Sensorino.h>

//To wake up, connect pin to 5V, then remove it
int pin = 5;

void setup() {
  Serial.begin(57600);
  Serial.println("starting");
  byte myAddr[4] = {1,100,2,200};
  configure(9, 10, 2, myAddr);
  if(!start()) Serial.println("Cannot start!");
  wakeUpPeriodically(2); //every 16 seconds
  wakeUpOnPinChange(pin);
}

void loop() {
  Serial.print("Going to sleep in 5 seconds");
  delay(1000);
  Serial.print(".");
  delay(1000);
  Serial.print(".");
  delay(1000);
  Serial.print(".");
  delay(1000);
  Serial.print(".");
  delay(1000);
  Serial.println(".");
  Serial.println("Zzzzz");
  delay(500);
  sleep();
  //Now sleeps until watchdog or interrupt
  Serial.println();
  Serial.println("Woken up!");
}
