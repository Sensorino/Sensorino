/** Trivial JSON messages parser for Sensorino base.
 * messages follow a known structure depending on the type of message:
 * { "publish": { "address": [1,2,3,4], "serviceID": 2, "serviceInstanceID": 0, "data": { ..... } } }
 * { "set": { "address": [1,2,3,4], "serviceID": 2, "serviceInstanceID": 0, "data": { ..... } } }
 * { "request": { "address": [1,2,3,4], "serviceID": 2, "serviceInstanceID": 0, "data": { ..... } } }
 * { "control": { "address": [1,2,3,4], "type": "ADVERT", "data": { ..... } }
 * { "control": { "address": [1,2,3,4], "type": "PING", "data": { ..... } } }
 * { "control": { "address": [1,2,3,4], "type": "PONG", "data": { ..... } } }
 * { "error": { "address": [1,2,3,4], "type": "SERVICE_UNAVAILABLE", "data": { ..... } } }
 *
 * Author: Dario Salvi (dariosalvi78 at gmail dot com)
 *
 * Licensed under the GPL license http://www.gnu.org/copyleft/gpl.html
 */

#include <Sensorino_Protocol.h>


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
 * @return the pointer where the data starts (aftert the :), NULL if not found
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

/** Creates the JSON control message.
 * @param buffer a buffer where to hold the string
 * @param ctrltype control type
 * @param address the source or destination address
 * @param data any associated data, formatted in JSON, NULL if none
 * @return the actual number of characters, including the terminating character
 */
int makeJSONControl(char* buffer, ControlType ctrltype, byte* address, char* data);

/** Creates the JSON Error message.
 * @param buffer a buffer where to hold the string
 * @param errtype error type
 * @param address the source or destination address
 * @param data any associated data, formatted in JSON, NULL if none
 * @return the actual number of characters, including the terminating character
 */
int makeJSONError(char* buffer, ErrorType errtype, byte* address, char* data);

/** Creates the JSON Service message.
 * @param buffer a buffer where to hold the string
 * @param msgtype message type, should be PUBLISH, REQUEST or SET
 * @param address the source or destination address
 * @param serviceID the service identifier
 * @param serviceInst ID the service instance identifier
 * @param data any associated data, formatted in JSON, NULL if none
 * @return the actual number of characters, including the terminating character
 */
int makeJSONService(char* buffer, MessageType msgtype, byte* address, unsigned int serviceID, byte serviceInstID, char* data);

/** Reads the strings coming from the serial and calls the parsers.
 * It listens for the time specified in millis and does not exit
 * until that time has been reached.
 * Spaces, tabs and new lines are removed automatically.
 * @param millis the time to wait until some data is found
 */
void readSerial(int millis);

/** Adds an handler of control JSON messages.
 * @param h is a function that treats the message
 */
void setJSONControlMessageHandler(void (*h)(ControlType ctrlt, byte* address, char* data));

/** Adds an handler of error JSON messages.
 * @param h is a function that treats the message
 */
void setJSONErrorMessageHandler(void (*h)(ErrorType errt, byte* address, char* data));

/** Adds an handler of service JSON messages.
 * @param h is a function that treats the message
 */
void setJSONServiceMessageHandler(void (*h)(MessageType msgtype, byte* address,unsigned int serviceID, byte servInstID, char* message));
