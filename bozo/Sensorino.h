#ifndef BASE_H
#define BASE_H


//Maximum number of instantiable base services (excluding control ones)
#define MAX_SERVICES 20

class Sensorino
{
    public:

        Sensorino();
        
        void onRadioMessage(uint8_t *rawData);
        void setAddress(uint8_t address);
        uint8_t getAddress();

        bool sendMessage(Message m);

        void addService(Service s);
        void deleteService(Service s);

    private: 
        uint8_t address;

        Service[MAX_SERVICES] services;
        uint8_t servicesNum=0;

        RHReliableDatagram *manager;


}


#endif // whole file
