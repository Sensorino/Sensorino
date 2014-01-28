#ifndef SENSORINO_h
#define SENSORINO_h

#include "NRF24.h"

//The pipe used for broadcast messages
#define BRADCAST_PIPE 0;

//The pipe used for private messages
#define PRIVATE_PIPE 1;


/** Sensorino, library that builds on top of the nRF24L01 chip.
 *
 *
 *
 */
class Sensorino : public NRF24
{
    public:

        /** Constructor. Multiple instances are allowed with their own
         * chip enable and slave select pin.
         * init() must be called to initialise the interface and the radio module
         * @param chipEnablePin the Arduino pin to use to enable the chip for transmit/receive
         * @param chipSelectPin the Arduino pin number of the output to use to select the NRF24 before
         * @param broadcastAdd the address used for broadcast
         * @param thisAddress the address of this node
         * accessing it
         */
        Sensorino(uint8_t chipEnablePin, uint8_t chipSelectPin,
                  char* broadcastAdd, char* thisAddress);

        /** Constructor. Default values are used for chip enable and slave and
         * broadcast address.
         * init() must be called to initialise the interface and the radio module
         * @param thisAddress the address of this node
         * accessing it
         */
        Sensorino(char* thisAddress);

        /** Initialises this instance and the radio module connected to it.
         * Initializes the SPI
         * - Set the radio to powerDown
         * @return  true if everything was successful
         */
        boolean init();

        /** Gets the internal voltage.
        * Works with a 328 or 168 only.
        * The voltage is returned in millivolts.
        * From: https://code.google.com/p/tinkerit/wiki/SecretVoltmeter
        */
        int readVcc();

        /** Gets the internal temperature of the MCU.
        * Temperature is returned in milli-°„C. So 25000 is 25°„C.
        * From: https://code.google.com/p/tinkerit/wiki/SecretThermometer
        */
        int readTemp();

    protected:
    private:
        /** The broadcast address.
         */
        char* broadCastAddress;

        /** The address of this node
         */
        char* thisAddress;
};

#endif // SENSORINO_h
