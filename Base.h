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

//JSON rough parseer

/** Reads the strings coming from the serial and calls the parsers.
 * It listens for the time specified in millis and does not exit
 * until that time has been reached.
 * Spaces and new lines are removed automatically.
 * @param millis the time to wait until some data is found
 */
void readSerial(int millis);

/** Adds an handler of JSON messages.
 * @param word is the JSON message type
 * @param h is a function that treats the message
 * @return true on success
 */
boolean addJSONDataHandler(char* word, void (*h)(char* message));

/** Separates an array into an array of strings.
 * @param line a pointer to the line to be analyzed
 * @param arr a pre-initialized array of char*
 * @param len the length of the resulting array
 */
void JSONtoStringArray(char* line, char** arr, int* len);

/** Looks for the start of a data, given its name
 * example: search for "data" returns a pointer next to the : after "data" has been found
 * @param line the line where to look into
 * @param dataname the name of the data without the ".." and :
 * @return the pointer where the data starts (aftert the :)
 */
char* JSONsearchDataName(char* line, char* dataname);

/** Converts a JSON property to a unsigned long.
 * @param line the line that contains the property
 * @param dataName the property to be parsed, should not include the \"...\":
 * @return the parsed unsigned long
 */
unsigned long JSONtoULong(char* line, char* dataName);

/** Converts a JSON property to a double.
 * @param line the line that contains the property
 * @param dataName the property to be parsed, should not include the \"...\":
 * @return the parsed double
 */
double JSONtoDouble(char* line, char* dataName);

/** Converts a JSON property to boolean.
 * @param line the line to be parsed
 * @param dataName the name of the property
 * @return the boolean value
 */
boolean JSONtoBoolean(char* line, char* dataName);


#endif // BASE_H
