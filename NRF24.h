/** NRF24 control library.
 * This library implements the functionalities of the NRF24L01+ chip without
 * (almost) any further abstraction.
 *
 * Author: Dario Salvi (dariosalvi78 at gmail dot com)
 *
 * Based on the work of Mike McCauley (mikem at airspayce dot com)
 * Original code Id: NRF24.h,v 1.1 2012/12/12 11:17:16 mikem Exp mikem
 * Available at http://www.airspayce.com/mikem/arduino/NRF24/
 *
 * Differences with the original library:
 * - uses java doc documentation style
 * - adds reliability: almost every command is checked after being executed
 * - supports all pipes (not only 0 and 1)
 * - adds several functionalities that were missing in the original version
 * Still unsupported:
 * - acks with payload
 *
 * Licensed under the GPL license http://www.gnu.org/copyleft/gpl.html
 */

#ifndef NRF24_h
#define NRF24_h

#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <wiring.h>
#include <pins_arduino.h>
#endif

// These defs cause trouble on some versions of Arduino
#undef round
#undef double

// This is the bit in the SPI address that marks it as a write
#define NRF24_SPI_WRITE_MASK 0x80

// This is the maximum message length that can be supported by this library. Limited by
// the suported message lengths oin the nRF24
// Can be pre-defined to a smaller size (to save SRAM) prior to including this header
#ifndef NRF24_MAX_MESSAGE_LEN
#define NRF24_MAX_MESSAGE_LEN 32
#endif

// Keep track of the mode the NRF24 is in
#define NRF24_MODE_IDLE         0
#define NRF24_MODE_RX           1
#define NRF24_MODE_TX           2

// These values we set for FIFO thresholds are actually the same as the POR values
#define NRF24_TXFFAEM_THRESHOLD 4
#define NRF24_RXFFAFULL_THRESHOLD 55

// SPI Command names
#define NRF24_COMMAND_R_REGISTER                        0x00
#define NRF24_COMMAND_W_REGISTER                        0x20
#define NRF24_COMMAND_R_RX_PAYLOAD                      0x61
#define NRF24_COMMAND_W_TX_PAYLOAD                      0xa0
#define NRF24_COMMAND_FLUSH_TX                          0xe1
#define NRF24_COMMAND_FLUSH_RX                          0xe2
#define NRF24_COMMAND_REUSE_TX_PL                       0xe3
#define NRF24_COMMAND_R_RX_PL_WID                       0x60
#define NRF24_COMMAND_W_ACK_PAYLOAD(pipe)               (0xa8|(pipe&0x7))
#define NRF24_COMMAND_W_TX_PAYLOAD_NOACK                0xb0
#define NRF24_COMMAND_NOP                               0xff

// Register names
#define NRF24_REGISTER_MASK                             0x1f
#define NRF24_REG_00_CONFIG                             0x00
#define NRF24_REG_01_EN_AA                              0x01
#define NRF24_REG_02_EN_RXADDR                          0x02
#define NRF24_REG_03_SETUP_AW                           0x03
#define NRF24_REG_04_SETUP_RETR                         0x04
#define NRF24_REG_05_RF_CH                              0x05
#define NRF24_REG_06_RF_SETUP                           0x06
#define NRF24_REG_07_STATUS                             0x07
#define NRF24_REG_08_OBSERVE_TX                         0x08
#define NRF24_REG_09_RPD                                0x09
#define NRF24_REG_0A_RX_ADDR_P0                         0x0a
#define NRF24_REG_0B_RX_ADDR_P1                         0x0b
#define NRF24_REG_0C_RX_ADDR_P2                         0x0c
#define NRF24_REG_0D_RX_ADDR_P3                         0x0d
#define NRF24_REG_0E_RX_ADDR_P4                         0x0e
#define NRF24_REG_0F_RX_ADDR_P5                         0x0f
#define NRF24_REG_10_TX_ADDR                            0x10
#define NRF24_REG_11_RX_PW_P0                           0x11
#define NRF24_REG_12_RX_PW_P1                           0x12
#define NRF24_REG_13_RX_PW_P2                           0x13
#define NRF24_REG_14_RX_PW_P3                           0x14
#define NRF24_REG_15_RX_PW_P4                           0x15
#define NRF24_REG_16_RX_PW_P5                           0x16
#define NRF24_REG_17_FIFO_STATUS                        0x17
#define NRF24_REG_1C_DYNPD                              0x1c
#define NRF24_REG_1D_FEATURE                            0x1d

// These register masks etc are named wherever possible
// corresponding to the bit and field names in the nRF24L01 Product Specification
#define NRF24_MASK_RX_DR                                0x40
#define NRF24_MASK_TX_DS                                0x20
#define NRF24_MASK_MAX_RT                               0x10
#define NRF24_EN_CRC                                    0x08
#define NRF24_CRCO                                      0x04
#define NRF24_PWR_UP                                    0x02
#define NRF24_PRIM_RX                                   0x01

#define NRF24_ENAA_P5                                   0x20
#define NRF24_ENAA_P4                                   0x10
#define NRF24_ENAA_P3                                   0x08
#define NRF24_ENAA_P2                                   0x04
#define NRF24_ENAA_P1                                   0x02
#define NRF24_ENAA_P0                                   0x01

#define NRF24_ERX_P5                                    0x20
#define NRF24_ERX_P4                                    0x10
#define NRF24_ERX_P3                                    0x08
#define NRF24_ERX_P2                                    0x04
#define NRF24_ERX_P1                                    0x02
#define NRF24_ERX_P0                                    0x01

#define NRF24_AW                                        0x03
#define NRF24_AW_3_BYTES                                0x01
#define NRF24_AW_4_BYTES                                0x02
#define NRF24_AW_5_BYTES                                0x03

#define NRF24_ARD                                       0xf0
#define NRF24_ARC                                       0x0f

#define NRF24_RF_CH                                     0x7f

#define NRF24_CONT_WAVE                                 0x80
#define NRF24_RF_DR_LOW                                 0x20
#define NRF24_RF_DR_HIGH                                0x08
#define NRF24_PLL_LOCK                                  0x10
#define NRF24_PWR                                       0x06
#define NRF24_PWR_m18dBm                                0x00
#define NRF24_PWR_m12dBm                                0x02
#define NRF24_PWR_m6dBm                                 0x04
#define NRF24_PWR_0dBm                                  0x06

#define NRF24_RX_DR                                     0x40
#define NRF24_TX_DS                                     0x20
#define NRF24_MAX_RT                                    0x10
#define NRF24_RX_P_NO                                   0x0e
#define NRF24_STATUS_TX_FULL                            0x01


#define NRF24_PLOS_CNT                                  0xf0
#define NRF24_ARC_CNT                                   0x0f

#define NRF24_RPD                                       0x01

#define NRF24_TX_REUSE                                  0x40
#define NRF24_TX_FULL                                   0x20
#define NRF24_TX_EMPTY                                  0x10
#define NRF24_RX_FULL                                   0x02
#define NRF24_RX_EMPTY                                  0x01

#define NRF24_DPL_P5                                    0x20
#define NRF24_DPL_P4                                    0x10
#define NRF24_DPL_P3                                    0x08
#define NRF24_DPL_P2                                    0x04
#define NRF24_DPL_P1                                    0x02
#define NRF24_DPL_P0                                    0x01

#define NRF24_EN_DPL                                    0x04
#define NRF24_EN_ACK_PAY                                0x02
#define NRF24_EN_DYN_ACK                                0x01


/** Implementation of the nRF24L01 functions.
 *
 * This base class provides basic functions for sending and receiving addressed, reliable,
 * automatically acknowledged and retransmitted
 * datagrams via nRF24L01 of arbitrary length to 32 octets per packet.
 * On transmission, the addresses of this node defaults to 0x0000000000, unless changed.
 * The radio is configured to use Enhanced Shockburst with retransmits.
 */
class NRF24
{
public:

    /** Defines convenient values for setting data rates in setRF()
     */
    typedef enum
    {
        NRF24DataRate1Mbps = 0,   /**< 1 Mbps */
        NRF24DataRate2Mbps,       /**< 2 Mbps */
        NRF24DataRate250kbps      /**< 250 kbps */
    } NRF24DataRate;

    /** Convenient values for setting transmitter power in setRF()
     * These are designed to agree with the values for RF_PWR
     * To be passed to setRF();
     */
    typedef enum {
        NRF24TransmitPowerm18dBm = 0,   /**< -18 dBm */
        NRF24TransmitPowerm12dBm,       /**< -12 dBm */
        NRF24TransmitPowerm6dBm,        /**< -6 dBm */
        NRF24TransmitPower0dBm          /**< 0 dBm */
    } NRF24TransmitPower;

    /** Convenient values for setting address size.
     * To be passed to setAddressSize.
     */
    typedef enum {
        NRF24CRCNO =0, /**< No CRC */
        NRF24CRC1Byte , /**< 1 byte */
        NRF24CRC2Bytes /**< 2 bytes */
    } NRF24CRC;

    /** Convenient values for setting address size.
     * To be passed to setAddressSize.
     */
    typedef enum {
        NRF24AddressSize3Bytes =1, /**< 3 bytes */
        NRF24AddressSize4Bytes =2, /**< 4 bytes */
        NRF24AddressSize5Bytes =3 /**< 5 bytes */
    } NRF24AddressSize;

    /** Constructor. Multiple instances are allowed with its own
     * chip enable and slave select pin.
     * init() must be called to initialise the interface and the radio module
     * @param chipEnablePin the Arduino pin to use to enable the chip for transmit/receive
     * @param chipSelectPin the Arduino pin number of the output to use to select the NRF24 before
     * accessing it
     */
    NRF24(uint8_t chipEnablePin = 9, uint8_t chipSelectPin = 10);

    /** Initialises this instance and the radio module connected to it.
     * The following steps are taken:
     * - Set the chip enable and chip select pins to output LOW, HIGH respectively.
     * - Initialise the SPI output pins
     * - Initialise the SPI interface library to 8MHz (all SPI.setClockDivider()
     * after init() to lower the SPI frequency).
     * - Flush the receiver and transmitter buffers
     * - Power down the chip
     * @return  true if everything was successful
     */
    boolean init();

    /** Tells if the chip is powered up or sleeping.
     * @return true if powered up
     */
    boolean isPoweredUp();

    /** Sets the transmit and receive channel number.
     * The frequency used is (2400 + channel) MHz
     * @return true on success
     */
    boolean setChannel(uint8_t channel);

    /** Gets the transmission channel.
     *  @return channel
     */
    uint8_t getChannel();

    /** Gets the Received Power Detector.
     * @return 1 or 0
     */
    boolean getRPD();

    /** Sets the number of bytes used for CRC.
     * By default it's 1 byte CRC.
     * 0 bytes means no CRC, 1 byte or 2 bytes are allowed.
     * @param crc The number of bytes to be used. Use NRF24CRC enum
     * @return true on success
     */
    boolean setCRC(NRF24CRC crc);

    /** Gets the CRC as set in the chip
     * @return the CRC setting as NRF24CRC
     */
    NRF24CRC getCRC();

    /** Sets the size of the address: 3,4 or 5 bytes.
     * @param size use the enum NRF24AddressSize
     * @return true on success
     */
    boolean setAddressSize(NRF24AddressSize size);

    /** Gets the actual address size as in the register.
     * @return the size of the address
     */
    NRF24AddressSize getAddressSize();

    /** Set the Auto Retransmit Delay and Auto Retransmit Count
     * for Auto retransmission (ART). See section 7.4.2 in the NRF24 documentation
     * It may be very important to set an appropriate delay and count
     * for your application, especially with
     * 250kbps (i.e. slow) data rate. The defaults are OK for faster data rates. If
     * the delay is too short, the symptoms wil be unreliable transmission, or tranmsission failures
     * @param delay The number of 250 microsecond intervals to wait for an ACK.
     * @param count The number of retries to us.
     * @return true on success
     */
    boolean setTXRetries(uint8_t delay, uint8_t count = 3);

    /** Sets the next transmit address.
     * Uses this address in TX address and RX pipe 0 if ShockBurst is enabled
     * @param address The new address for transmitting.
     * The length must match the actual size.
     * @param autoack set to true if autoack is expected
     * @return true on success
     */
    boolean setTransmitAddress(uint8_t* address, boolean autoack);

    /** Gets the current transmit address.
     * @param addr a buffer where to store the address,
     * length must be the same as the actual address size.
     * @return true on success
     */
    boolean getTransmitAddress(uint8_t * addr);

    /** Sets the first len bytes of the address for the given NRF24 receiver pipe.
     * In case of pipe 2,3,4,5 it only sets the LSB, the rest is taken from pipe 1.
     * @param pipe The index of the pipe to set, from 0 to 5
     * @param address The address for receiving.
     * The size must be the same as the actual one.
     * @return true on success
     */
    boolean setPipeAddress(uint8_t pipe, uint8_t* address);

    /** Gets the address of the specified pipe.
     * @param pipe the pipe number, from 0 to 5
     * @param addr a buffer for the address,
     * the size must be the same as the actual address size
     * @return an array filled with the address, use getAddressSize to know the length
     */
    boolean getPipeAddress(uint8_t pipe, uint8_t * addr);

    /** Enables a pipe to receive data.
     * @param pipe the pipe number, from 0 to 5
     * @param autoAck enablesEnhanced Shockburst
     * @return true if it worked
     */
    boolean enablePipe(uint8_t pipe);

    /** Says if the pipe is enabled.
     * @param pipe pipe number from 0 to 5
     * @return true if enabled
     */
    boolean isPipeEnabled(uint8_t pipe);

    /** Enables auto acknoweldgment (Shockburst) on the pipe.
     * @param pipe pipe number
     * @return true if success
     */
    boolean enableAutoAck(uint8_t pipe);

    /** Tells if the pipe is wokring with auto ack.
     * @param pipe pipe number
     * @return true if enabled
     */
    boolean isAutoAckEnabled(uint8_t pipe);

    /** Sets the number of bytes transmitted
     * in each payload
     * @param pipe the pipe number
     * @param size Size of the transmitted payload in bytes,
     * 0 means dynamic payload
     * @return true on success
     */
    boolean setPayloadSize(uint8_t pipe, uint8_t size);

    /** Gets the size of the payload of a pipe.
     * @param pipe the pipe number
     * @return the size, 0 if set to dynamic
     */
    uint8_t getPayloadSize(uint8_t pipe);

    /** Sets the data rate and tranmitter power to use
     * @param data_rate The data rate to use for all packets transmitted and received. One of NRF24DataRate
     * @param power Transmitter power. One of NRF24TransmitPower.
     * @return true on success
     */
    boolean setRF(NRF24DataRate data_rate, NRF24TransmitPower power);

    /** Gets the current data rate.
     * @return the data rate as NRF24DataRate
     */
    NRF24DataRate getDatarate();

    /** Gets the current transmission power.
     * @return returns the NRF24TransmitPower enum
     */
    NRF24TransmitPower getTransmitPower();

    /** Sets the radio in power down mode.
     * Sets chip enable to LOW.
     * @return true on success
     */
    boolean powerDown();

    /** Sets the radio in RX mode.
     * Sets chip enable to HIGH to enable the chip in RX mode.
     * @return true on success
     */
    boolean powerUpRx();

    /** Sets the radio in TX mode.
     * Pulses the chip enable LOW then HIGH to enable the chip in TX mode.
     * @return true on success
     */
    boolean powerUpTx();

    /** Sends data to the address set by setTransmitAddress(). Blocks until the current message (if any)
     * has been transmitted.
     * Sets the radio to TX mode.
     * @param data Data bytes to send.
     * @param len Number of data bytes to set in teh TX buffer. The actual size of the
     * transmitted data payload is set by setPayloadSize
     * @param noack Optional parameter if true sends the message NOACK mode.
     * @return true on success
     */
    boolean send(uint8_t* data, uint8_t len, boolean noack = false);

    /** Sends a packet but does not wait after.
     * Sets the radio to TX mode.
     * @param data Data bytes to send.
     * @param len Number of data bytes to set in teh TX buffer. The actual size of the
     * transmitted data payload is set by setPayloadSize
     * @param noack Optional parameter if true sends the message NOACK mode.
     * @return true on success
     */
    void asyncSend(uint8_t* data, uint8_t len, boolean noack = false);

    /** Indicates if the chip is in transmit mode and
     * there is a packet currently being transmitted
     * @return true if the chip is in transmit mode and there is a transmission in progress
     */
    boolean isSending();

    /** Checks whether a received message is available.
     * This can be called multiple times in a timeout loop
     * @return true if a complete, valid message has been received and is able to be retrieved by
     * recv()
     */
    boolean available();

    /** Starts the receiver and blocks until a valid received
     * message is available.
     */
    void  waitAvailable();

    /** Starts the receiver and blocks until a received message is available or a timeout
     * @param timeout Maximum time to wait in milliseconds.
     * @return true if a message is available
     */
    boolean waitAvailableTimeout(uint16_t timeout);

    /** Turns the receiver on if it not already on.
     * If there is a valid message available, copy it to buf and return true
     * else return false.
     * If a message is copied, *len is set to the length (Caution, 0 length messages are permitted).
     * You should be sure to call this function frequently enough to not miss any messages
     * It is recommended that you call it in your main loop.
     * @param pipe a pointer to where to store the pipe number.
     * @param buf Location to copy the received message
     * @param len Pointer to available space in buf. Set to the actual number of octets copied.
     * @return true if a valid message was copied to buf
     */
    boolean recv(uint8_t* pipe, uint8_t* buf, uint8_t* len);

#ifdef DEBUG
     /** Prints the value of all chip registers
     * for debugging purposes
     * @return true on success
     */
    void printRegisters();
#endif

protected:

private:
    uint8_t _crc;
    uint8_t _chipEnablePin;
    uint8_t _chipSelectPin;


    /** Execute an SPI command that requires neither reading or writing
     * @param command the SPI command to execute, one of NRF24_COMMAND_*
     * @return the value of the device status register
     */
    uint8_t spiCommand(uint8_t command);

    /** Reads a single command byte from the NRF24
     * @param command Command number, one of NRF24_COMMAND_*
     * @return the single byte returned by the command
     */
    uint8_t spiRead(uint8_t command);

    /** Writes a single command byte to the NRF24
     * @param command Command number, one of NRF24_COMMAND_*
     * @param val The value to write
     * @return the value of the device status register
     */
    uint8_t spiWrite(uint8_t command, uint8_t val);

    /** Reads a number of consecutive bytes from a command using burst read mode
     * @param command Command number of NRF24_COMMAND_*
     * @param dest Array to write the bytes returned by the command to. Must be at least len bytes
     * @param len Number of bytes to read
     * @return the value of the device status register
     */
    void spiBurstRead(uint8_t command, uint8_t* dest, uint8_t len);

    /** Write a number of consecutive bytes to a command using burst write mode
     * @param command Command number of the first register, one of NRF24_COMMAND_*
     * @param src Array of bytes to write. Must be at least len bytes
     * @param len Number of bytes to write
     * @return the value of the device status register
     */
    uint8_t spiBurstWrite(uint8_t command, uint8_t* src, uint8_t len);

    /** Reads a single register from the NRF24
     * @param reg Register number, one of NRF24_REG_*
     * @return The value of the register
     */
    uint8_t spiReadRegister(uint8_t reg);

    /** Writes a single byte to the NRF24, and at the ame time reads the current STATUS register
     * @param reg Register number, one of NRF24_REG_*
     * @param val The value to write
     * @return the current STATUS (read while the command is sent)
     */
    uint8_t spiWriteRegister(uint8_t reg, uint8_t val);

    /** Reads a number of consecutive registers from the NRF24 using burst read mode
     * @param reg Register number of the first register, one of NRF24_REG_*
     * @param dest Array to write the register values to. Must be at least len bytes
     * @param len Number of bytes to read
     * @return the value of the device status register
     */
    void spiBurstReadRegister(uint8_t reg, uint8_t* dest, uint8_t len);

    /** Write a number of consecutive registers using burst write mode
     * @param reg Register number of the first register, one of NRF24_REG_*
     * @param src Array of new register values to write. Must be at least len bytes
     * @param len Number of bytes to write
     * @return the value of the device status register
     */
    uint8_t spiBurstWriteRegister(uint8_t reg, uint8_t* src, uint8_t len);

    /** Reads and returns the device status register NRF24_REG_02_DEVICE_STATUS
     * @return The value of the device status register
     */
    uint8_t statusRead();

    /** Flush the TX FIFOs
     * @return the value of the device status register
     */
    uint8_t flushTx();

    /** Flush the RX FIFOs
     * @return the value of the device status register
     */
    uint8_t flushRx();

    /** Tells if two addresses are the same.
     * @param addr1 the first address
     * @param addr2 the second address
     * @param len the length of the address
     * @return true if they are equals
     */
    boolean areAddressesEquals(uint8_t *addr1, uint8_t* addr2, uint8_t len);

    /** Prints the content of the registers in HEX.
    */
    void printRegisters();
};

#endif // NRF24_h
