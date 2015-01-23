/*
 * This sketch illustrates a basic node with one digital input pin used
 * as a sensor (e.g. a wall switch) and another as an actuator (e.g.
 * a relay controlling a lightbulb), joined with a hardcoded rule-engine
 * rule.  This is an advanced example and since the Rule Engine now
 * stores the rules in a non-volatile memory (EEPROM) there's no need
 * to re-create the rule on every restart as this sketch does.  You can
 * do it once over the radio protocol (for example from a web-base user
 * interface) and not hardcode it in your sketch.  However for illustration
 * we show how such a message is encoded and how it can be submitted to
 * the Sensorino library for processing.
 */
//#include <SPI.h>
//#include <RH_NRF24.h>
#include <Sensorino.h>
#include <RelayService.h>
#include <SwitchService.h>

#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>

#define MY_NODE_ADDR 10

#define RELAY_GPIO 8
#define SWITCH_GPIO 9

Sensorino s;
RelayService rs(2, RELAY_GPIO, 0);
SwitchService ss(3, SWITCH_GPIO);

static uint8_t msg[] = {
  0,   // from: The Base (0)
  10,  // to: This node (10)
  3,   // type: SET
  1,   // message id
  1,   // type: ServiceId
  1,   // tuple length
  1,   // ServiceId: RuleService
  50,  // type: count
  1,   // tuple length
  5,   // ruleid 5
  3,   // type: Expression
  9,   // tuple length
  6,   // OP_NE
  3,   // variable (first operand for OP_NE above)
  3,   // switchservice
  52,  // switch
  0,   // 0th switch
  4,   // previous_value (second operand for OP_NE above)
  3,   // switchservice
  52,  // switch
  0,   // 0th switch
  2,   // type: Message
  3,   // tuple length
  1,   // type: ServiceId
  1,   // length
  2,   // ServiceId: RelayService
};

/*
 * You can also use the following syntax to generate the 9-byte sequence
 * that defines the example rule's condition:
 *   (data:3:Switch:0 != prev:3:Switch:0)
 * In other words the rule is triggered whenever the new value of the
 * switch represented by ServiceId 3 is not equal to its previous value.
 */
#if 0
DEF_EXPR(
  DEF_BINARY(OP_NE,
    DEF_CURRENT(3, SWITCH, 0),
    DEF_PREVIOUS(3, SWITCH, 0),
  )
)
#endif

void setup() {
  __asm__ volatile ("wdr");
  wdt_disable();

  s.setAddress(MY_NODE_ADDR);

  Message m(msg, sizeof(msg));
  s.handleMessage(m);

  sleep_enable();

  /*
   * Power-save automatically shuts down all peripherals except these:
   *   WDT
   *   Timer2
   *   Pin-change detection
   *   TWI
   * Standby:
   *   Timers
   *   WDT
   *   Pin-change detection
   *   TWI
   * Power-down:
   *   WDT
   *   Pin-change detection
   *   TWI
   */
  ACSR = 1 << ACD; /* ACD == 1 powers the ac off */
  ADCSRA = 0x00; /* ADEN == 0 powers adc off */
  WDTCSR = 0x00; /* WDE == WDIE == 0 powers wdt off */
  /* For Active mode: */
  power_twi_disable();
  power_adc_disable();
}

void loop() {
  if (Timers::pending())
    set_sleep_mode(SLEEP_MODE_STANDBY);
  else
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  /* Sequence recommended in <avr/sleep.h> */
  cli();
  //sleep_bod_disable();
  sei();
  sleep_cpu();
}
