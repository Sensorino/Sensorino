/** Sensorino communication library.
 * This library goes on top of the nRF24L01.
 * Decisions taken:
 * - pipe 0 is used as broadcast pipe, with shared address and no acks
 * - pipe 1 is used as private address
 * - nodes send their address
 * - addresses are 4 bytes long
 * - CRC is 2 bytes
 * - 2Mbps, 750us ack time, 3 retries
 *
 * Author: Dario Salvi (dariosalvi78 at gmail dot com)
 *
 * Licensed under the GPL license http://www.gnu.org/copyleft/gpl.html
 */
#ifndef SENSORINO_PROTOCOL_H_INCLUDED
#define SENSORINO_PROTOCOL_H_INCLUDED

#include <nRF24.h>

//The pipe used for broadcast messages
#define BROADCAST_PIPE 0

//Default broadcast address
#define BROADCAST_ADDR 0,255,0,255

//The pipe used for private messages
#define PRIVATE_PIPE 1

//Default adrress of the base station
#define BASE_ADDR 255,0,255,0

//Default radio channel
#define RF_CHANNEL 10


/** Local variable used to store the broadcast address.
*/
extern byte broadCastAddress[4];

/** Local variable used to store the base address.
*/
extern byte baseAddress[4];

/** Local variable used to store the sensorino address.
*/
extern byte thisAddress[4];


/** Configures and starts the radio.
 * init() must be called to initialise the interface and the radio module
 * @param chipEnablePin the Arduino pin to use to enable the chip for transmit/receive
 * @param chipSelectPin the Arduino pin number of the output to use to select the NRF24 before
 * @param myAddress the address of this node
 * @return true on success
 */
boolean startRadio(byte chipEnablePin, byte chipSelectPin, byte irqpin, byte myAddress[]);

/** Defines the types of message that can be sent in the sensorinos network.
 * There are four main categories of messages:
 * ERR: used to send error messages
 * CONTROL: messages used to setup and control the network
 * PUBLISH: messages used to publish data and status updates
 * SET: messages used to control devices
 * REQUEST: messages used to pull a state or data update
 */
typedef enum MessageType {
        ERR = 0,
        CTRL,
        PUBLISH,
        SET,
        REQUEST
} MessageType;

#define MAX_SENSORINO_PAYLOAD_LEN NRF24_MAX_MESSAGE_LEN-5

/** Sends a message to another sensorino.
 * @param broadcast true if bradcast
 * @param destination address of the destination
 * @param msgType type of message
 * @param len length of the payload
 * @return true if sent
 */
boolean send(boolean broadcast, byte* destination, MessageType msgType, byte* data, int len);

/** Receives a message.
 * @param timeoutMS a timeout in milliseconds
 * @param broadcast tells if the message was in broadcast
 * @param sender the sender address
 * @param msgType the message type
 * @param databuffer a buffer where the payload is put
 * @param len the length of the payload
 */
boolean receive(unsigned int timeoutMS, boolean* broadcast, byte* sender, MessageType* msgType,
                byte* databuffer, int* len);


/** Control message type
 * PING: sends the node a ping an waits for a pong
 * ADVERT: the node self declares
 * TIMESYNCH: the node requests a time synchronization
 */
typedef enum ControlType{
    PING = 0,
    ADVERT,
    TIMESYNCH
} ControlType;

#define MAX_CONTROL_PAYLOAD_LEN MAX_SENSORINO_PAYLOAD_LEN-1

/** Creates the payload for a control message.
 * @param payload a buffer where to contain the payload, already allocated
 * @param controlT the control message type
 * @param data any associated data to send
 * @param len if any data, its length
 */
void makeCtrlPayload(byte* payload, ControlType controlT, byte* data, int len);

/** Parses the payload of a control message.
 * @param ctrlpayload the payload to be parsed
 * @param payloadlen the length of the payload
 * @param controlT returns the control message type
 * @param data returns any associated, must be pre-allocated
 * @param datalen returns data length
 */
void unmakeCtrlPayload(byte* ctrlpayload, int payloadlen, ControlType* controlT, byte* data, int* datalen);

/** Utility function that creates and sends a control message.
 * @param broadcast true if broadcast
 * @param destination address of the receiver
 * @param controlT the control message type
 * @param data any associated data to send
 * @param len if any data, its length
 */
boolean sendControl(boolean broadcast, byte* destination, ControlType controlT, byte* data, int len);

/** Error messages
 * SERVICE_UNAVAILABLE: the requested service is not available in the node
 * DATA_FORMAT_UNSUPPORTED: the requested data format is not supported on the node
 * CANNOT_PARSE_DATA: the sent data is not readable
 */
typedef enum ErrorType{
    SERVICE_UNAVAILABLE = 0,
    DATA_FORMAT_UNSUPPORTED,
    CANNOT_PARSE_DATA,
} ErrorType;

#define MAX_ERROR_PAYLOAD_LEN MAX_SENSORINO_PAYLOAD_LEN - 1

/** Creates the payload for a error message.
 * @param payload a buffer where to contain the payload, already allocated
 * @param errT the error message type
 * @param data any associated data to send
 * @param len if any data, its length
 */
void makeErrorPayload(byte* payload, ErrorType errT, byte* data, int len);

/** Parses the payload of an error message.
 * @param errpayload the payload to be parsed
 * @param payloadlen the length of the payload
 * @param errT returns the error message type
 * @param data returns any associated, must be pre-allocated
 * @param datalen returns data length
 */
void unmakeErrorPayload(byte* errpayload, int payloadlen, ErrorType* errT, byte* data, int* datalen);

/** Utility function that creates and sends an error message.
 * @param broadcast true if broadcast
 * @param destination address of the receiver
 * @param errorT the error type
 * @param data any associated data to send
 * @param len if any data, its length
 */
boolean sendError(boolean broadcast, byte* destination, ErrorType errorT, byte* data, int len);

/** Defines the format used to represent data.
 * ADHOC: is an ad-hoc structure
 * ATOMIC: is key-value format
 * CIAO: is a binary format from the Cosa framework
 */
typedef enum DataFormat{
    ADHOC = 0,
    ATOMIC,
    CIAO
} DataFormat;

#define MAX_SERVICE_PAYLOAD_LEN MAX_SENSORINO_PAYLOAD_LEN - 4

/** Creates the payload for a service message.
 * @param payload a buffer where to contain the payload, already allocated
 * @param serviceType the service id
 * @param serviceInstance the id of the instance
 * @param dataFthe data format type
 * @param data any associated data to send
 * @param len if any data, its length
 */
void makeServicePayload(byte* payload, unsigned int serviceType, byte serviceInstance,
                        DataFormat dataF, byte* data, int len);

/** Parses the payload of a service message.
 * @param payload the payload to be parsed
 * @param payloadlen the length of the payload
 * @param serviceID returns the service id
 * @param serviceInstanceID returns the service instance id
 * @param dataF returns the data format
 * @param data returns any associated, must be pre-allocated
 * @param datalen returns data length
 */
void unmakeServicePayload(byte* payload, int payloadlen, unsigned int* serviceID,
                            byte* serviceInstanceID, DataFormat* dataF, byte* data, int* datalen);

/** Utility function that creates and sends a service message.
 * @param broadcast true if broadcast
 * @param destination address of the receiver
 * @param serviceType the service id
 * @param serviceInstance the id of the instance
 * @param dataFthe data format type
 * @param data any associated data to send
 * @param len if any data, its length
 */
boolean sendService(boolean broadcast, byte* destination, unsigned int serviceType, byte serviceInstance,
                        DataFormat dataF, byte* data, int len);

#endif // SENSORINO_PROTOCOL_H_INCLUDED
