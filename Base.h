/** Library that implements all the functionalities of the Base node.
 * It imports the functionalities from Sensorino_JSON for managing messaging with the base
 * and those included in Sensorino_Protocol for connectivity
 * plus, it adds the support for BaseService objects
 *
 * Author: Dario Salvi (dariosalvi78 at gmail dot com)
 *
 * Licensed under the GPL license http://www.gnu.org/copyleft/gpl.html
 */
#ifndef BASE_H_INCLUDED
#define BASE_H_INCLUDED

#include <Sensorino_Protocol.h>
#include <Sensorino_JSON.h>
#include <BaseService.h>

//Maximum nuber of instantiable base services (excluding control ones)
#define MAX_SERVICES 20

/** Adds a SensorinoService to the pool of used ones
 * @param serv a pointer to a BaseService
 * @return true on success
 */
boolean addService(BaseService* serv);

/** Initializes all registered services
 * @return true if initialization was OK
 */
boolean initServices();

/** Runs all registered services
 * @return true if running was OK for all
 */
boolean runServices();

/** Waits for incoming packets and dispatches them to services
 * this function should be always called
 * @param timeoutMS a timeout in milliseconds
 * @return true if a packet was received
 */
boolean receive(unsigned int timeoutMS);

#endif // BASE_H_INCLUDED
