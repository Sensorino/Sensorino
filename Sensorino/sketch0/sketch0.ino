/*
 * Basic theoretical example that sets up a node at address 10 with one
 * "Relay" service representing a digital output pin.  The main loop then
 * flips the value of the digital pin (which can be connected to an
 * actual relay controlling some home appliance, for example) every two
 * seconds, by simulating a radio message from the Sensorino server
 * that executes the relay command.  Of course you normally don't need
 * to do that in the sketch, it'd be a task for the server or the local
 * rule engine that are easily programmable from a nice User Interface.
 */
//#include <SPI.h>
//#include <RH_NRF24.h>
#include <Sensorino.h>
#include <RelayService.h>
#include <Timers.h>

Sensorino s;
RelayService rs(2, 13, 0); /* The onboard LED is the "relay" */

void setup() {
  s.setAddress(10);
}

void loop() {
  /* Switch the relay on and off (LED should blink on/off) */
  static int on;
  on = !on;

  Timers::delay(2000);
  __asm__ volatile ("wdr");

  Message m(s.getBaseAddress(), s.getAddress());
  m.setType(SET);
  m.addIntValue(SERVICE_ID, rs.getId());
  m.addBoolValue(SWITCH, on);

  s.handleMessage(m);
}
