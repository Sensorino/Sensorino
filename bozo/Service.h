/** Abstract class of a generic service.
 * It stores some few local values and imposes an interface to be followed.
 *
 * Author: Dario Salvi (dariosalvi78 at gmail dot com)
 *
 * Licensed under the GPL license http://www.gnu.org/copyleft/gpl.html
 */
#ifndef SERVICE_H_INCLUDED
#define SERVICE_H_INCLUDED

#include <stdint.h>

#include "Message.h"

class Service {
public:

    /** Constructor of a generic Service.
     * @param serviceID locally unique identifier of this service
     */
    Service(int _id);

    int getId(void) { return id; }

    void handleMessage(Message *message);

protected:
    uint8_t id;

    /* Services need to implement some of the following two: */
    virtual void onSet(Message *message) = 0;
    virtual void onRequest(Message *message) = 0;

    /* Service implementations use this to start a new PUBLISH message.
     * When done constructing the contents, they'll call Message->send();
     */
    Message *publish(Message *message = NULL);

    /* Service implementations use this to start a new ERR message.
     * When done constructing the contents, they'll call Message->send();
     */
    Message *err(Message *message = NULL);

    /* Start a new message addressed at the Base or at the sender of
     * the original message. */
    Message *startBaseMessage(MessageType type, Message *orig);

private:
};

#endif // SERVICE_H_INCLUDED
/* vim: set sw=4 ts=4 et: */
