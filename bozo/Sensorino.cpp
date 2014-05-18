#include "Sensorino.h"
#include "Message.h"

Sensorino::Sensorino(){
    // Singleton instance of the radio driver
    RH_NRF24 driver;

    // Class to manage message delivery and receipt, using the driver declared above
    manager = new RHReliableDatagram(driver, address);
}

bool Sensorino::sendMessage(Message m){

    // Send a message to manager_server
    if (*manager.sendtoWait(m.getRawData(), sizeof(m.getRawData()), m.getDstAddress()))
    {
        // Now wait for a reply from the server
        uint8_t len = sizeof(buf);
        uint8_t from;   
        if (*manager.recvfromAckTimeout(buf, &len, 2000, &from))
        {
            Serial.print("got reply from : 0x");
            Serial.print(from, HEX);
            Serial.print(": ");
            Serial.println((char*)buf);
            return true;
        }
        else
        {
            Serial.println("No reply, is nrf24_reliable_datagram_server running?");
            return false;
        }
    }
    else{
        Serial.println("sendtoWait failed");
        return false;
    }


}

/* Potnetially-temporary global single sensorino instance.  We can pass this
 * pointer around when calling service constructors later, for the moment
 * let's use a global...
 */
Sensorino *sensorino;
