/** Time service: a service for synchronizing clocks.
 * This service works on the broadcast channel (pipe 0).
 * Every isntance can work as a client or server.
 * A messagge is sent by the node that wants to set the time.
 * All nodes that have the time set should answer.
 *
 * Author: Dario Salvi (dariosalvi78 at gmail dot com)
 *
 * Licensed under the GPL license http://www.gnu.org/copyleft/gpl.html
 */

#ifndef TIMESERVICE_H
#define TIMESERVICE_H

#include <Sensorino.h>
#include <Base.h>

#define TIME_SERVICE 1

/** Packet containing unix timestamp
 */
typedef struct {
    unsigned long timestamp;
} timePacket;

/** Gets the local time.
 * @return the time as unix timestamp
 */
unsigned long getTime();

/** Sets the local time.
 * @param unixtime the time stamp in unix format
 */
void setTime(unsigned long unixtime);

/** Asks the time to other nodes, the time is set if received
 */
void askTime();

/** Serves the time if available
 */
void serveTime();

/** Time protocol with server through serial.
 * Used by the Base node.
 * Arduino sends { "command" : "getTime" }
 * Server answers: { "time" : 1391796357 } (seconds since Jan 01 1970)
 * Arduino sets internal time
 */
void askServerTime();

/** Time protocol with server through serial.
 * Arduino sends { "command" : "getTime" }
 * Server answers: { "time" : 1391796357 } (seconds since Jan 01 1970)
 * Arduino sets internal time
 */
void parseServerTime(String line);

#endif // TIMESERVICE_H
