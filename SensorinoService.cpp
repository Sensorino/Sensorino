/** Abstract class of a service instantiated in a Sensorino (vs the one in the base).
 * It stores some few local values and imposes an interface to be followed.
 *
 * Author: Dario Salvi (dariosalvi78 at gmail dot com)
 *
 * Licensed under the GPL license http://www.gnu.org/copyleft/gpl.html
 */

#include <Service.h>
#include <SensorinoService.h>

SensorinoService::SensorinoService(unsigned int serviceTypeID, byte siid, int sp, int wkpn, ...): Service(serviceTypeID) {
    serviceInstanceID = siid;
    sleepPeriod = sp;
    wakeupPinsN = wkpn;

    va_list list;
    va_start(list, wkpn);
    for(int i = 0; i<wkpn; i++){
       wakeupPins[i] = va_arg(list, int);
    }
    va_end(list);
};


byte SensorinoService::getServiceInstanceID(){
    return serviceInstanceID;
};

int SensorinoService::getSleepPeriod(){
    return sleepPeriod;
};

int SensorinoService::getWakeupPinsNumber(){
    return wakeupPinsN;
};

int* SensorinoService::getWakeupPins(){
    return wakeupPins;
};
