#ifndef SERVICES_h
#define SERVICES_h

#include "Sensorino.h"

#define TIME_SERVICE 1 /**< service number for time */

class Services
{
    public:
        static unsigned long getTime();
        static void setTime(unsigned long unixtime);
        static void askTime();
        static void serveTime(byte * sender);
    protected:
    private:
        static unsigned long lastUnixTime;
        static unsigned long lastTimeStamp;

};

/** THE instance, Arduino style
 */
extern Services service;

#endif // SERVICES_h
