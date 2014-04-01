/** Library that implements all the functionalities of a Sensorino node.
 * It imports the functionalities from Sensorino_Energy for energy management
 * and those included in Sensorino_Protocol for connectivity
 * plus, it adds the support for SensorinoService objects
 *
 * Author: Dario Salvi (dariosalvi78 at gmail dot com)
 *
 * Licensed under the GPL license http://www.gnu.org/copyleft/gpl.html
 */
#include <Sensorino_Energy.h>
#include <Sensorino_Protocol.h>
#include <Service.h>
#include <SensorinoService.h>

//Maximum nuber of instantiable sensorino services (excluding control ones)
#define MAX_SERVICES 10

/** Adds a SensorinoService to the pool of used ones
 * @param serv a pointer to a SensorinoService
 * @return true on success
 */
boolean addService(SensorinoService* serv);

/** Initializes all registered services
 * @return true if initialization was OK
 */
boolean initServices();

/** Runs all registered services
 * @return true if running was OK for all
 */
boolean runServices();

/** Sleeps the sensorino for the minimum time, according to services needs
 * If one or more services have peridical wakeups, the sleeping period will last
 * according to the maximum common divider of all periods
 * It sleeps until a wakeup is triggered by a pin if all services follow this strategy
 * It does not sleep if at least one service has a 0 as sleeping period
 */
void sleep();

/** Waits for incoming packets and dispatches them to services
 * this function should be always called, but if a sleep intervenes, packets will get lost meanwhile
 * @param timeoutMS a timeout in milliseconds
 * @return true if a packet was received
 */
boolean receive(unsigned int timeoutMS);
