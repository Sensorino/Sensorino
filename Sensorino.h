/** Sensorino library.
 * This library abstracts the nRF24L01.
 * Decisions taken:
 * - pipe 0 is used as broadcast pipe, with shared address and no acks
 * - pipe 1 is used as private address
 * - nodes send their address
 * - addresses are 4 bytes long
 * - CRC is 2 bytes
 * - 2Mbps, 750us ack time, 3 retries
 * The library also implements a set of "services" on top of basic communication means.
 *
 * Author: Dario Salvi (dariosalvi78 at gmail dot com)
 *
 * Licensed under the GPL license http://www.gnu.org/copyleft/gpl.html
 */

#ifndef SENSORINO_H
#define SENSORINO_H

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


extern byte broadCastAddress[4];
extern byte baseAddress[4];
extern byte thisAddress[4];

/** Counter of the wake ups produced by the watchdog.
 * Beign signed it lasts 72 hours max, after which it is resetted.
 */
extern volatile int periodicWakeUpCounter;


/** Configures Sensorino.
 * init() must be called to initialise the interface and the radio module
 * @param chipEnablePin the Arduino pin to use to enable the chip for transmit/receive
 * @param chipSelectPin the Arduino pin number of the output to use to select the NRF24 before
 * @param myAddress the address of this node
 */
void configure(byte chipEnablePin, byte chipSelectPin, byte irqpin, byte myAddress[]);

/** Initialises this instance and the radio module connected to it.
 * Initializes the SPI
 * - Set the radio to powerDown
 * @return true on success
 */
boolean start();

/** Put the Sensorino into sleep mode.
 * Powers down everything, radio and MCU.
 * First call wakeUpOnPinChange or wakeUpPeriodically or it
 * won't wakeup until a reset !
 */
void sleep();

/** Wakes up the Sensorino if the level of a pin changes.
 * @param pin the pin, can be 2 or 3
 */
void wakeUpOnPinChange(byte pin);

/** Wakes the Sensorino up every 8 seconds.
 * You can use periodicWakeUps to check how many times it has been waken up.
 * @param _8secsmult multiple of 8 seconds after which the sensorino should really wakeup
 * being an unsigned 8 bits int, the maximum is around 30 minutes
 */
void wakeUpPeriodically(byte _8secsmult);

/** Composes the base packet with sender information.
 * @param buffer a buffer where to store the final packet
 * @param service the service number
 * @param data the data associated with the packet
 * @param len the length of the data
 */
void composeBasePacket(byte* buffer, unsigned int service, byte* data, int len);

/** Decomposes the base packet.
 * @param packet the raw packet
 * @param totlen the length of the raw packet
 * @param sender a buffer where the sender address will be written, must be of size 4
 * @param service the service number
 * @param data a buffer where the data will be written. Must be of size 26.
 * @param len the length of the data without base packet
 */
void decomposeBasePacket(byte* packet, int totlen, byte* sender, unsigned int* service, byte* data, int* len);

/** Sends a packet to the base.
 * @param service the service id
 * @param data data associated
 * @param len the length of the data
 * @return true on success
 */
boolean sendToBase(unsigned int service, byte* data, int len);

/** Sends a packet to broadcast.
 * @param service the service id
 * @param data data associated
 * @param len the length of the data
 * @return true on success
 */
boolean sendToBroadcast(unsigned int service, byte* data, int len);

/** Waits to receive a packet on a pipe, if received it extracts sender and service.
 * @param timeout a timeout in millis
 * @param broadcast tells if the packet is in the broadcast channel or the unicast
 * @param sender a buffer or 4 bytes where the sender address will be stored
 * @param service here the service number will be stored
 * @param data here the data will be stored
 * @param len the length of the data
 * @return true if data has been received
 */
boolean receive(unsigned int timeout, boolean* broadcast, byte* sender, unsigned int* service, byte* data, int* len);


#endif // SENSORINO_H
