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

#define INTERNALS_SERVICE 2

/** Packet containing the measured voltage (*1000) and temperature (*1000)
 */
typedef struct {
    unsigned long timestamp; /**< unix timestamp */
    int vcc; /**< voltage in millivolts */
    int temperature; /**< temperature as read by the ADC */
} internalsPacket;

/** Gets the internal voltage.
 * The voltage is returned in millivolts.
 * From: https://code.google.com/p/tinkerit/wiki/SecretVoltmeter
 */
int readVcc();

/** Gets the internal temperature of the MCU.
 * The value is returned as the raw value from the ADC.
 * From: http://playground.arduino.cc/Main/InternalTemperatureSensor
 */
int readTemp();

/** Sends the internals to the base
 * @param the internals information
 * @return true on success
 */
boolean sendInternals(internalsPacket pkt);

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
