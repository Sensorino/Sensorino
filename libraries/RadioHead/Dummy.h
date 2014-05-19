// Dummy.h
//
// Copyright (C) 2014 Mike McCauley
// $Id: Dummy.h,v 1.5 2014/04/23 00:23:47 mikem Exp $

// Works with any serial port. Tested with Arduino Mega connected to Serial1
// Also works with 3DR Radio V1.3 Telemetry kit (serial at 57600baud)

#ifndef Dummy_h
#define Dummy_h

#include <RHGenericDriver.h>
// Maximum message length (including the headers) we are willing to support
#define RH_SERIAL_MAX_PAYLOAD_LEN 64

// The length of the headers we add.
// The headers are inside the payload and are therefore protected by the FCS
#define RH_SERIAL_HEADER_LEN 4

// This is the maximum message length that can be supported by this library. 
// It is an arbitrary limit.
// Can be pre-defined to a smaller size (to save SRAM) prior to including this header
// Here we allow for 4 bytes of address and header and payload to be included in the 64 byte encryption limit.
// the one byte payload length is not encrpyted
#ifndef RH_SERIAL_MAX_MESSAGE_LEN
#define RH_SERIAL_MAX_MESSAGE_LEN (RH_SERIAL_MAX_PAYLOAD_LEN - RH_SERIAL_HEADER_LEN)
#endif

class Dummy : public RHGenericDriver
{
public:
    /// Constructor
    /// \param[in] serial Reference to the HardwareSerial port which will be used by this instance
    Dummy();

    /// Initialise the Driver transport hardware and software.
    /// Make sure the Driver is properly configured before calling init().
    /// \return true if initialisation succeeded.
    virtual bool init();

    /// Tests whether a new message is available
    /// from the Driver. 
    /// On most drivers, this will also put the Driver into RHModeRx mode until
    /// a message is actually received bythe transport, when it wil be returned to RHModeIdle.
    /// This can be called multiple times in a timeout loop
    /// \return true if a new, complete, error-free uncollected message is available to be retreived by recv()
    virtual bool available();

    /// Turns the receiver on if it not already on.
    /// If there is a valid message available, copy it to buf and return true
    /// else return false.
    /// If a message is copied, *len is set to the length (Caution, 0 length messages are permitted).
    /// You should be sure to call this function frequently enough to not miss any messages
    /// It is recommended that you call it in your main loop.
    /// \param[in] buf Location to copy the received message
    /// \param[in,out] len Pointer to available space in buf. Set to the actual number of octets copied.
    /// \return true if a valid message was copied to buf
    virtual bool recv(uint8_t* buf, uint8_t* len);

    /// Waits until any previous transmit packet is finished being transmitted with waitPacketSent().
    /// Then loads a message into the transmitter and starts the transmitter. Note that a message length
    /// of 0 is NOT permitted. 
    /// \param[in] data Array of data to be sent
    /// \param[in] len Number of bytes of data to send (> 0)
    /// \return true if the message length was valid and it was correctly queued for transmit
    virtual bool send(const uint8_t* data, uint8_t len);

    /// Returns the maximum message length 
    /// available in this Driver.
    /// \return The maximum legal message length
    virtual uint8_t maxMessageLength();


};

/// @example serial_reliable_datagram_client.pde
/// @example serial_reliable_datagram_server.pde

#endif
