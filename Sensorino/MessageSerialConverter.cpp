#include <MessageSerialConverter>


void MessageSerialConverter::printMessageOnSerial(Message m){
    aJsonObject *msg = aJson.createObject();

    aJson.addItemToObject(msg, "type", m.typeAsString());
    // TODO src, dest ....
    
    


    aJson.print(msg, &serial_stream);
    Serial.println(); /* Add newline. */
    aJson.deleteItem(msg);
}

void serializePublish(aJsonObject *msg, Message m)
{
    // now serialize payload

}



