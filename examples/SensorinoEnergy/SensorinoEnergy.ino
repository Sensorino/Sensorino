/** An example about Sensorino Energy management.
 * The Sensorino is put to sleep for 10 seconds
 */
#include <SPI.h>
#include <nRF24.h>
#include <Sensorino_Energy.h>

const int wakeuppin = 19;
const int seconds = 10;

void setup() {
  Serial.begin(57600);
  Serial.println("Sensorino energy management example");
  //need to init nrf24 because sleep turns it off
  nRF24.configure(9, 10, 2);
  nRF24.init();

  pinMode(wakeuppin, INPUT);
}

void loop() {
    int pins[1];
    pins[0] = wakeuppin;

    unsigned long time = getTotalSleepSeconds() + (millis() / 1000);
    Serial.print("time: ");
    Serial.println(time);
    Serial.print("going to sleep for ");
    Serial.print(seconds);
    Serial.println(" seconds...");
    delay(100);
    sleepUntil(seconds, pins, 1);

    Serial.println("Wakeup !");
    Serial.print("wait 2 secs");
    delay(1000);Serial.print(".");
    delay(1000);Serial.println(".");
}

