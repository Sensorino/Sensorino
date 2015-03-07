/*
 * This sketch is a real-life example of a node with a few preprogrammed
 * services for: 7 wall switches and 4 relays controlling lights in four
 * rooms at my home.  Some additional Atmega328 energy-saving instructions
 * are present.
 *
 * The switches are later programmed to control relevant lightbulbs by
 * adding simple rules.  The rules are not hardcoded here, they are
 * managed remotely from the central User Interface.  The rules are
 * executed locally however, once created they do not depend on radio
 * communication to work.
 */
#include <Sensorino.h>
#include <RelayService.h>
#include <SwitchService.h>
#include <OnchipThermometerService.h>

#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>

#define MY_NODE_ADDR 10

#define SWITCH0_GPIO 1
#define SWITCH1_GPIO 6
#define SWITCH2_GPIO 7
#define SWITCH3_GPIO 8
#define SWITCH4_GPIO 9
#define SWITCH5_GPIO 18
#define SWITCH6_GPIO 19
#define RELAY0_GPIO 2
#define RELAY1_GPIO 3
#define RELAY2_GPIO 4
#define RELAY3_GPIO 5

/* On-chip thermistor's offset in C determined experimentally */
#define TEMP_OFFSET 60

Sensorino s;
RelayService rs0(10, RELAY0_GPIO, 0);
RelayService rs1(11, RELAY1_GPIO, 0);
RelayService rs2(12, RELAY2_GPIO, 0);
RelayService rs3(13, RELAY3_GPIO, 0);
SwitchService ss0(20, SWITCH0_GPIO);
SwitchService ss1(21, SWITCH1_GPIO);
SwitchService ss2(22, SWITCH2_GPIO);
SwitchService ss3(23, SWITCH3_GPIO);
SwitchService ss4(24, SWITCH4_GPIO);
SwitchService ss5(25, SWITCH5_GPIO);
SwitchService ss6(26, SWITCH6_GPIO);
OnchipThermometerService ots(30, TEMP_OFFSET);

void setup() {
  __asm__ volatile ("wdr");
  wdt_disable();

  s.setAddress(MY_NODE_ADDR);

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
  power_timer0_disable();
  power_timer2_disable();
  power_usart0_disable();
}

void loop() {
  if (Timers::pending())
    set_sleep_mode(SLEEP_MODE_IDLE);
  else
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_cpu();
}
