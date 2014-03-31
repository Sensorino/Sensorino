/** Abstract class of a generic service.
 * It stores some few local values and imposes an interface to be followed.
 *
 * Author: Dario Salvi (dariosalvi78 at gmail dot com)
 *
 * Licensed under the GPL license http://www.gnu.org/copyleft/gpl.html
 */
#ifndef SERVICE_H_INCLUDED
#define SERVICE_H_INCLUDED

#include <Sensorino_Protocol.h>

class Service{

public:

    /** Constructor of a generic Service.
     * @param serviceTypeID the unique identifier of the service type
     * @param serviceInstanceID a number that identifies the local instance of the service
     * used if more than an instance is used
     * @param sleepPeriod
     */
    Service(unsigned int serviceTypeID);

    /** Retrieves the service type ID
    */
    unsigned int getServiceTypeID();

    /**
     */
    virtual void handlePacket(boolean broadcast, MessageType msgType, DataFormat format,
                              byte* data, unsigned int len);

    /**
     */
    virtual void run();

private:
    /** The service number */
    unsigned int serviceTypeID;

};

#endif // SERVICE_H_INCLUDED
