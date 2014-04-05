/** Abstract class of a service instantiated in the Base (vs the one in the Sensorino).
 * It stores some few local values and imposes an interface to be followed.
 *
 * Author: Dario Salvi (dariosalvi78 at gmail dot com)
 *
 * Licensed under the GPL license http://www.gnu.org/copyleft/gpl.html
 */

#include <Service.h>
#include <Base.h>

BaseService::BaseService(unsigned int serviceTypeID):Service(serviceTypeID){};
