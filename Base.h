/** Base, a base for sensorionos.
 * Adds communication to a server on the serial line.
 * Serial line messages are either debugging messages, either JSON messages.
 * JSON messages always start with { and end with }
 * In principle JSON messages are sent on one line.
 *
 * Author: Dario Salvi (dariosalvi78 at gmail dot com)
 *
 * Licensed under the GPL license http://www.gnu.org/copyleft/gpl.html
 */
#ifndef BASE_H
#define BASE_H

#include <nRF24.h>
#include <Sensorino.h>

/** Configures Base.
 * @param chipEnablePin the Arduino pin to use to enable the chip for transmit/receive
 * @param chipSelectPin the Arduino pin number of the output to use to select the NRF24 before
 */
void configureBase(byte chipEnablePin, byte chipSelectPin, byte irqpin);

/** Initialises this instance and the radio module connected to it.
 * Initializes the SPI
 * - Set the radio to powerDown
 * @return true on success
 */
boolean startBase();

/** Sends a packet to a sensorino.
 * @param address the address of the receiver
 * @param service the service id
 * @param data data associated
 * @param len the length of the data
 * @return true on success
 */
boolean sendToSensorino(byte address[],word service, byte* data, int len);

/** Converts a JSON property to a unsigned long.
 * @param line the line that contains the property
 * @param data the property to be parsed, it must include the \"...\":
 * @return the parse unsigned long
 */
unsigned long JSONtoULong(char* line, char* data);


#endif // BASE_H
