/** Abstract class of a service instantiated in a Sensorino (vs the one in the base).
 * It stores some few local values and imposes an interface to be followed.
 *
 * Author: Dario Salvi (dariosalvi78 at gmail dot com)
 *
 * Licensed under the GPL license http://www.gnu.org/copyleft/gpl.html
 */
#ifndef SENSORINOSERVICE_H_INCLUDED
#define SENSORINOSERVICE_H_INCLUDED

#include <Service.h>

#define MAX_WAKEUP_PINS 5


class SensorinoService: public Service{

public:

    /** Constructor of a generic Service.
     * @param serviceTypeID the unique identifier of the service type
     * @param serviceInstanceID a number that identifies the local instance of the service
     * used if more than an instance is used
     * @param sleepPeriod
     */
    SensorinoService(unsigned int serviceTypeID, byte serviceInstanceID,
            int sleepPeriod, int wakeupPinsN, ...);

    /** Retrieves the service instance ID
     * @return the service instance ID
     */
    byte getServiceInstanceID();

    /** Retrieves the sleeping period of the service
     * @return the sleeping period in seconds,
     * beign <0 (sleep forever), 0 (no sleep) or >0 (sleep a number of secs)
     */
    int getSleepPeriod();

    /** If the service has to be woken up by pins, this indicates how many
     * @return the number of pins to be woken up by
     */
    int getWakeupPinsNumber();

    /** The pins to be got woken up by
     * @return an array of integers indicating the pin numbers
     */
    int* getWakeupPins();

    /** Virtual function used to handle a message coming from the Base.
     * @param broadcast is true if the message was in broadcast
     * @param msgType the message type
     * @param format the data format
     * @param data the associated data if any
     * @param len the length of the data
     */
    virtual void handleMessage(boolean broadcast, MessageType msgType, DataFormat format,
                              byte* data, int len)=0;

private:

    /** The service instance */
    byte serviceInstanceID;

    /** The sleep period */
    int sleepPeriod;

    /** number of wakeup pins*/
    int wakeupPinsN;

    /** Array containing wakeuppins */
    int wakeupPins[MAX_WAKEUP_PINS];

};

#endif // SENSORINOSERVICE_H_INCLUDED
