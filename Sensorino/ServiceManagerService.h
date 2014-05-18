#include "Sensorino.h"
#include "Service.h"

class ServiceManagerService : public Service {
public:
    ServiceManagerService() : Service(0) {
    }

protected:
    void onRequest(Message *message) {
        Message *msg = publish(message);
        int svc_num = 0;

        while (1) {
            Service *svc = sensorino->getServiceByNum(svc_num++);
            if (!svc)
                break;

            msg->addIntValue(SERVICE_ID, svc->getId());
        };

        msg->send();
    }
};
/* vim: set sw=4 ts=4 et: */
