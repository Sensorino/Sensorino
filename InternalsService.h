/** Internals service: a service for sending information about the Sensorino.
 * This service works on the unicast channel (pipe 1).
 * Three data are supported now:
 * timestamp: the time of the node
 * temperature: internal temperature of the MCU
 * VCC: the voltage running in the MCU
 *
 * Author: Dario Salvi (dariosalvi78 at gmail dot com)
 *
 * Licensed under the GPL license http://www.gnu.org/copyleft/gpl.html
 */

#ifndef INTERNALSSERVICE_H
#define INTERNALSSERVICE_H

#include <Sensorino.h>
#include <Base.h>

#define INTERNALS_SERVICE 1

/** Packet containing the measured voltage (*1000) and temperature (*1000)
 */
typedef struct {
    unsigned long timestamp; /**< unix timestamp */
    int vcc; /**< voltage in millivolts */
    int temp; /**< temperature in milli C */
} internalsPacket;

/** Gets the internal voltage.
 * Works with atmega 328 or 168 only.
 * The voltage is returned in millivolts.
 * From: https://code.google.com/p/tinkerit/wiki/SecretVoltmeter
 */
int readVcc();

/** Gets the internal temperature of the MCU.
 * Temperature is returned in milli-¡ãC. So 25000 is 25¡ãC.
 * From: https://code.google.com/p/tinkerit/wiki/SecretThermometer
 */
int readTemp();

/** Sends the internals to the base
 * The timestamp of the measurement, 0 unknown.
 */
void sendInternals(unsigned long timestamp);

/** Converts bytes back to internals
 */
internalsPacket parseInternals(byte* data);

/** Sends the data of Internals to the Server
 * uses a JSON message.
 * @param address the address of the sender, 4 bytes
 * @param ints the internals packet
 */
void serverSendInternals(byte* address, internalsPacket ints);

#endif // INTERNALSSERVICE_H
