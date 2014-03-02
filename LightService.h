/** Light intensity service: a service for sending information about brightness.
 * This service works on the unicast channel (pipe 1).
 * Two data are supported:
 * timestamp: the time of the node
 * light: the raw ADC value of the light intensity
 *
 * Author: Dario Salvi (dariosalvi78 at gmail dot com)
 *
 * Licensed under the GPL license http://www.gnu.org/copyleft/gpl.html
 */
#ifndef LIGHTSERVICE_H_INCLUDED
#define LIGHTSERVICE_H_INCLUDED

#include <Sensorino.h>
#include <Base.h>

#define LIGHT_SERVICE 4


typedef struct {
    unsigned long timestamp; /**< unix timestamp */
    unsigned int light; /**< light intensity, 0 (no light) to 1023 (maximum light) */
} lightPacket;

/** Sends the temperature and humidity to the base
 * @param pkt containing light information
 * @return true on success
 */
boolean sendLight(lightPacket pkt);

/** Converts bytes back to lightPacket
 */
lightPacket parseLight(byte* data);

/** Sends the data of light to the Server
 * uses a JSON message.
 * @param address the address of the sender, 4 bytes
 * @param lght the lightPacket packet
 */
void serverSendLight(byte* address, lightPacket lght);

#endif // LIGHTSERVICE_H_INCLUDED
