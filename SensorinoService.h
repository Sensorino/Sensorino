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
    */
    byte getServiceInstanceID();

    int getSleepPeriod();

    int getWakeupPinsNumber();

    int* getWakeupPins();

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
