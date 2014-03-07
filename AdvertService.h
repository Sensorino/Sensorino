/** A service for advertising the presence and the services of a Sensorino.
 * This service works on the unicast channel (pipe 1).
 * It sends a list of services identifiers.
 *
 * Author: Dario Salvi (dariosalvi78 at gmail dot com)
 *
 * Licensed under the GPL license http://www.gnu.org/copyleft/gpl.html
 */

#ifndef ADVERTSERVICE_H_INCLUDED
#define ADVERTSERVICE_H_INCLUDED

#include <Sensorino.h>
#include <Base.h>

#define ADVERT_SERVICE 0


typedef struct {
    byte servicesNumber; /**< number of services */
    unsigned int services[]; /**< a list of services IDs */
} advertPacket;

/** Sends the advert packet with the list of services
 * @param pkt list of services IDs
 * @return true on success
 */
boolean sendAdvert(advertPacket pkt);

/** Converts bytes back to advertPacket
 */
advertPacket parseAdvert(byte* data);

/** Sends the data of advert to the Server
 * Uses a JSON message.
 * @param address the address of the sender, 4 bytes
 * @param adv the advertPacket packet
 */
void serverSendAdvert(byte* address, advertPacket adv);

#endif // ADVERTSERVICE_H_INCLUDED
