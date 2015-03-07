#include "Sensorino.h"
#include "Service.h"
#include "Timers.h"

class ServiceManagerService : public Service {
public:
    ServiceManagerService() : Service(0) {
        /* Within one second from now announce the Sensorino and its
         * services to the Base.  We have no good way to know when all
         * the services have been registered so give all other code one
         * second from the moment Sensorino constructor calls us and
         * fire the announcement.
         */
        Timers::setObjTimeout(ServiceManagerService::initialAnnounce, F_TMR);
    }

protected:
    void onRequest(Message *message) {
        Message *msg = publish(message);
        int svc_num = 0;

        while (1) {
            Service *svc = sensorino->getServiceByNum(svc_num++);
            if (!svc)
                break;

            if (svc->getId() != id)
                msg->addIntValue(Data::SERVICE_ID, svc->getId());
        };

        msg->send();
    }

    void initialAnnounce(void) {
        onRequest(NULL);
    }
};
/* vim: set sw=4 ts=4 et: */
