/** Switch service: a service for sending information about a on/off switch.
 * This service works on the unicast channel (pipe 1).
 * Two data are supported:
 * timestamp: the time of the node
 * status: on or off
 *
 * Author: Dario Salvi (dariosalvi78 at gmail dot com)
 *
 * Licensed under the GPL license http://www.gnu.org/copyleft/gpl.html
 */
#ifndef SWITCHSERVICE_H_INCLUDED
#define SWITCHSERVICE_H_INCLUDED

#include <Sensorino.h>
#include <Base.h>

#define SWITCH_SERVICE 4

/** Packet containing the switch info
 */
typedef struct {
    unsigned long timestamp; /**< unix timestamp */
    boolean status; /**< status, on or off */
} switchPacket;

/** Sends the switch data to the base
 * @param pkt containing switch information
 * @return true on success
 */
boolean sendSwitch(switchPacket pkt);

/** Converts bytes back to switchPacket
 */
switchPacket parseSwitch(byte* data);

/** Sends the data of switchPacket to the Server
 * uses a JSON message.
 * @param address the address of the sender, 4 bytes
 * @param sw the switchPacket packet
 */
void serverSendSwitch(byte* address, switchPacket sw);

/** Parses a switch command coming from the server.
 * Example: { "Switch"{ "address": [1,2,3,4], "status": true }}
 * @param address where the address of the sensorino will be written, it must initialized
 * @return a switchPacket with filled parsed data
 */
switchPacket serverParseSwitch(char* line, byte* address);

#endif // SWITCHSERVICE_H_INCLUDED
