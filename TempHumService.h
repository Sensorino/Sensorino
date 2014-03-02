/** Temperature and humidity service: a service for sending weather information.
 * This service works on the unicast channel (pipe 1).
 * Three data are supported now:
 * timestamp: the time of the node
 * temperature: environment temeprature in °„C x 10
 * humidity: the humidity in %
 *
 * Author: Dario Salvi (dariosalvi78 at gmail dot com)
 *
 * Licensed under the GPL license http://www.gnu.org/copyleft/gpl.html
 */
#ifndef TEMPHUMSERVICE_H_INCLUDED
#define TEMPHUMSERVICE_H_INCLUDED

#include <Sensorino.h>
#include <Base.h>

#define TEMPHUM_SERVICE 3


/** Packet containing the measured temperature (*10) and humidity (in %)
 */
typedef struct {
    unsigned long timestamp; /**< unix timestamp */
    int temperature; /**< temperature, multiplied by 10 */
    int humidity; /**< humidity in % */
} tempHumPacket;

/** Sends the temperature and humidity to the base
 * @param pkt containing temperatura and humidity information
 * @return true on success
 */
boolean sendTempHum(tempHumPacket pkt);

/** Converts bytes back to tempHumPacket
 */
tempHumPacket parseTempHum(byte* data);

/** Sends the data of TempHumPacket to the Server
 * uses a JSON message.
 * @param address the address of the sender, 4 bytes
 * @param ints the internals packet
 */
void serverSendTempHum(byte* address, tempHumPacket ints);

#endif // TEMPHUMSERVICE_H_INCLUDED
