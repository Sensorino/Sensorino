//#include <SPI.h>
//#include <RH_NRF24.h>
#include <Sensorino.h>
#include <RelayService.h>
#include <SwitchService.h>
#include <Timers.h>

#define RELAY_GPIO 8
#define SWITCH_GPIO 9

Sensorino s;
RelayService rs(2, RELAY_GPIO, 0);
SwitchService ss(3, SWITCH_GPIO);

void setup() {
  s.setAddress(10);
}

void loop() {
  __asm__ volatile ("wdr");
}
