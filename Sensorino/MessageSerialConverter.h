#ifndef MESSAGE_SERIAL_CONVERTER_H
#define MESSAGE_SERIAL_CONVERTER_H


#include "Message.h"
#include <aJSON.h>



class MessageSerialConverter{
    public:
        MessageSerialConverter();
        void printMessageOnSerial(Message m);

    private:
        aJsonStream serial_stream(&Serial);


}

        
#endif // whole file



