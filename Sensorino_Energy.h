/** Sensorino energy management library.
 * It uses the features of the ATMega chip to power down the MCU
 * It alsos powerdowns the radio chip
 *
 * Author: Dario Salvi (dariosalvi78 at gmail dot com)
 *
 * Licensed under the GPL license http://www.gnu.org/copyleft/gpl.html
 */
#ifndef SENSORINO_ENERGY_H_INCLUDED
#define SENSORINO_ENERGY_H_INCLUDED

#include <nRF24.h>

/** Resets sensorino.
 */
void reset();

/** Put the Sensorino into sleep mode until wither a certain time has passed or a pin has changed.
 * Powers down everything, radio and MCU.
 * @param seconds the number of seconds after which we want the board to wakeup.
 * 0 means: don't sleep at all
 * a number <1 means don't care, sleep until someone else wakes it up
 * @param pins a set of pin numbers on the change of which we want the board to wake up
 * @param pinN the number of pins, if <=0 the pins arenot considered
 */
void sleep(int seconds, int* pins, int pinN);

/** Gives the number of seconds the Sensorino has been sleeping since it has been switched on
 * @return the total number of seconds it has slept
 */
unsigned long getTotalSleepSeconds();

#endif // SENSORINO_ENERGY_H_INCLUDED
