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
    Service();

    /** Retrieves the service type ID
    */
    unsigned int getInstanceId();

    /** Virtual function used to initialize the service
     * @return shall return true if everything went OK
     */
    virtual boolean init()=0;

    /** Virtual function used to be run periodically or on a pin wakeup
     * @return shall return true if everything was OK
     */
    virtual boolean run()=0;
    boolean isInteractive();
    boolean setInteractive(bool interactive);

private:
    uint8_t instanceId;
    bool interactive;

};

#endif // SERVICE_H_INCLUDED
