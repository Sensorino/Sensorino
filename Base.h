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
#ifndef BASE_h
#define BASE_h

#include "NRF24.h"
#include "Sensorino.h"

/** Base, a base for sensorionos.
 * Adds communication to a server on the serial line.
 */
class Base : public Sensorino
{
    public:

        /** Configures Sensorino.
         * init() must be called to initialise the interface and the radio module
         * @param chipEnablePin the Arduino pin to use to enable the chip for transmit/receive
         * @param chipSelectPin the Arduino pin number of the output to use to select the NRF24 before
         */
        static void configure(byte chipEnablePin, byte chipSelectPin);

        /** Initialises this instance and the radio module connected to it.
         * Initializes the SPI
         * - Set the radio to powerDown
         * @return true on success
         */
        static boolean init();

        /** Sends a packet to a sensorino.
         * @param address the address of the receiver
         * @param service the service id
         * @param data data associated
         * @param len the length of the data
         * @return true on success
         */
        static boolean sendToSensorino(byte address[],word service, byte* data, int len);



        ///////////////////////////////
        // Server side communication //
        ///////////////////////////////

        /** Reads a line from the serial.
         * @param buffer needs a buffer where to store characters
         * @return a String object
         */
        String readLineFromSerial(char* buffer);

        /** Interprets lines sent by the server over Serial port.
         * @param line the received line
         */
        void parseServerLine(String line);

        //Broadcast services:

        //Time service

        /** Time protocol with server through serial.
         * Arduino sends #getTime
         * Server answers: #time 1391429779 (seconds since Jan 01 1970)
         * Arduino sets internal time
         */
        void askServerTime();

        /** Time protocol with server through serial.
         * Arduino sends #getTime
         * Server answers: #time 1391429779 (seconds since Jan 01 1970)
         * Arduino sets internal time
         */
        void parseServerTime(String line);

        //Sensorino to base services:

        //Internals service

        /** Sends the information of internals to the server.
         * @param address the address of the sensorino
         * @param pkt the information
         */
         void serverSendInternals(byte * address, internalsPacket pkt);

    protected:

    private:

};

/** THE instance, Arduino style
 */
extern Base base;

#endif // BASE_h
