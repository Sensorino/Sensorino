//#include <SPI.h>
//#include <RH_NRF24.h>
#include <Sensorino.h>
#include <RelayService.h>
#include <Timers.h>
#include <aJSON.h>

Sensorino s;
RelayService rs(1, 13, 0); /* The onboard LED is the "relay" */

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

  const uint8_t *raw = m.getRawData();
  int len = m.getRawLength();

  s.handleMessage(raw, len);
}
