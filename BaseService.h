/** Abstract class of a service instantiated in the Base (vs the one in the Sensorino).
 * It stores some few local values and imposes an interface to be followed.
 *
 * Author: Dario Salvi (dariosalvi78 at gmail dot com)
 *
 * Licensed under the GPL license http://www.gnu.org/copyleft/gpl.html
 */
#ifndef BASESERVICE_H_INCLUDED
#define BASESERVICE_H_INCLUDED

#include <Service.h>
#include <Sensorino_JSON.h>


class BaseService: public Service{

public:

    BaseService(unsigned int serviceTypeID);

    /** Virtual function used to handle a message coming from a Sensorino.
     * @param broadcast is true if the message was in broadcast
     * @param source the source address
     * @param msgType the message type
     * @param serviceInstanceID the service instance id
     * @param format the data format
     * @param data the associated data if any
     * @param len the length of the data
     */
    virtual void handleMessage(boolean broadcast, byte* source, MessageType msgType, byte serviceInstanceID,
                               DataFormat format, byte* data, int len)=0;

    /** Virtual function that handles a JSON message coming from the server.
     * @param msgtype type of message
     * @param address the address to which the messages goes
     * @param servInstID service instance id
     * @param message the JSON message
     */
    virtual void handleJSONMessage(MessageType msgtype, byte* address, byte servInstID, char* message)=0;
};


#endif // BASESERVICE_H_INCLUDED
