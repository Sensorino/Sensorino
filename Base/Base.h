#ifndef BASE_H
#define BASE_H


//Maximum nuber of instantiable base services (excluding control ones)
#define MAX_SERVICES 20

class Base
{
    public:
        void run();
    
        // stuff received from radio

        void onSerialJsonMessage();
        void onRadioMessage();
        

    private: 

        int listenTimeout=5;

        void listen(int timeout);
        void onSerialEvent();

        // stuff received from radio
        void onRadioPublish();
        void onRadioPing();
        void onRadioPong();


        // stuff received from serial
        void onSerialRequest();
        void onSerialSet();
        void onSerialPing();
        void onSerialPong();

}


#endif // whole file
