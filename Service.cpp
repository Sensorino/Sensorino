/** Abstract class of a generic service.
 * It stores some few local values and imposes an interface to be followed.
 *
 * Author: Dario Salvi (dariosalvi78 at gmail dot com)
 *
 * Licensed under the GPL license http://www.gnu.org/copyleft/gpl.html
 */
#include <Service.h>

Service::Service(unsigned int sN){
    serviceTypeID = sN;
}

unsigned int Service::getServiceTypeID(){
    return serviceTypeID;
};



