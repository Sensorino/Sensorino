#ifndef BASE_H
#define BASE_H


//Maximum number of instantiable base services (excluding control ones)
#define MAX_SERVICES 20

class Base
{
    public:
        void run();
    
        // stuff received from radio
        void onSerialJsonMessage();
        void onRadioMessage();
        

    private: 

        Service[MAX_SERVICES] services;

        int listenTimeout=5;
        void listen(int timeout);

        void onSerialEvent();

        // stuff received from serial
        void onSerialRequest();
        void onSerialSet();
        void onSerialPing();
        void onSerialPong();

}


#endif // whole file
