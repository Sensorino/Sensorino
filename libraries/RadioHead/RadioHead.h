// RadioHead.h
// Author: Mike McCauley (mikem@airspayce.com)
// Copyright (C) 2014 Mike McCauley
// $Id: RadioHead.h,v 1.13 2014/05/03 00:20:36 mikem Exp mikem $

/// \mainpage RadioHead Packet Radio library for embedded microprocessors
///
/// This is the RadioHead Packet Radio library for embedded microprocessors.
/// It provides a complete object-oriented library for sending and receiving packetized messages
/// via a variety of common data radios on a range of embedded microprocessors.
///
/// The version of the package that this documentation refers to can be downloaded 
/// from http://www.airspayce.com/mikem/arduino/RadioHead/RadioHead-1.8.zip
/// You can find the latest version at http://www.airspayce.com/mikem/arduino/RadioHead
///
/// You can also find online help and disussion at 
/// http://groups.google.com/group/radiohead-arduino
/// Please use that group for all questions and discussions on this topic. 
/// Do not contact the author directly, unless it is to discuss commercial licensing.
/// Before asking a question or reporting a bug, please read http://www.catb.org/esr/faqs/smart-questions.html
///
/// \par Overview
///
/// RadioHead consists of 2 main sets of classes: Drivers and Managers.
///
/// - Drivers provide low level access to a range of different packet radios and other packetized message transports.
/// - Managers provide high level message sending and receiving facilities for a range of different requirements.
///
/// Every RadioHead program will have an instance of a Driver to provide access to the data radio or transport, 
/// and a Manager that uses that driver to send and receive messages for the application. The programmer is required
/// to instantiate a Driver and a Manager, and to initialise the Manager. Thereafter the facilities of the Manager
/// can be used to send and receive messages.
///
/// It is also possible to use a Driver on its own, without a Manager, although this only allows unaddressed, 
/// unreliable transport via the Driver's facilities.
///
/// In some specialised cases, it is possible to instantiate more than one Driver and more than one Manager.
///
/// A range of different common embedded microprocessor platforms are supported, allowing your project to run
/// on your choice of processor.
///
/// Example programs are included to show the main modes of use.
///
/// \par Drivers
///
/// The following Drivers are provided:
///
/// - RH_RF22
/// Works with Hope-RF
/// RF22B based radio modules, and compatible chips and modules, 
/// including the RFM22B transceiver module such as 
/// this bare module: http://www.sparkfun.com/products/10153
/// and this shield: http://www.sparkfun.com/products/11018. Supports GFSK, FSK and OOK. Access to other chip 
/// features such as on-chip temperature measurement, analog-digital 
/// converter, transmitter power control etc is also provided.
///
/// - RH_RF69 
/// Works with Hope-RF
/// RF69B based radio modules, such as the RFM69 module, (as used on the excellent Moteino and Moteino-USB 
/// boards from LowPowerLab http://lowpowerlab.com/moteino/ )
/// and compatible chips and modules such as RFM69W, RFM69HW, RFM69CW, RFM69HCW (Semtech SX1231, SX1231H). 
/// Supports GFSK, FSK.
///
/// - RH_NRF24
/// Works with Nordic nRF24 based 2.4GHz radio modules, such as nRF24L01 and others.
///
/// - RH_NRF905
/// Works with Nordic nRF905 based 433/868/915 MHz radio modules.
///
/// - RH_ASK
/// Works with a range of inexpensive ASK (amplitude shift keying) RF transceivers such as RX-B1 
/// (also known as ST-RX04-ASK) receiver; TX-C1 transmitter and DR3100 transceiver. Supports ASK (OOK).
///
/// - RH_Serial
/// Works with RS232, RS422, RS485, RS488 and other point-to-point and multidropped serial connections, 
/// or with TTL serial UARTs such as those on Arduino and many other processors,
/// or with data radios with a 
/// serial port interface. RH_Serial provides packetization and error detection over any hardware or 
/// virtual serial connection. 
///
///
/// Drivers can be used on their own to provide unaddressed, unreliable datagrams. 
/// All drivers have the same identical API.
/// Or you can use any Driver with any of the Managers described below.
///
/// We welcome contributions of well tested and well documented code to support other transports.
///
/// \par Managers
///
/// The following Mangers are provided:
///
/// - RHDatagram
/// Addressed, unreliable variable length messages, with optional broadcast facilities.
///
/// - RHReliableDatagram
/// Addressed, reliable, retransmitted, acknowledged variable length messages.
///
/// - RHRouter
/// Multi-hop delivery from source node to destination node via 0 or more intermediate nodes.
///
/// - RHMesh
/// Multi-hop delivery with automatic route discovery and rediscovery.
///
/// Any Manager may be used with any Driver.
///
/// \par Platforms
/// 
/// A range of platforms is supported:
///
/// - Arduino and the Arduino IDE (version 1.0 to 1.5.5 and later)
/// Including Diecimila, Uno, Mega, Leonardo, Yun etc. http://arduino.cc/, Also similar boards such as 
/// Moteino http://lowpowerlab.com/moteino/ etc.
///
/// - ChipKit Uno32 board and the MPIDE development environment
/// http://www.digilentinc.com/Products/Detail.cfm?Prod=CHIPKIT-UNO32
///
/// - Maple and Flymaple boards with libmaple and the Maple-IDE development environment
/// http://leaflabs.com/devices/maple/ and http://www.open-drone.org/flymaple
///
/// - Teensy including Teensy 3.1 built using Arduino IDE 1.0.5 with 
///   teensyduino addon 1.18 and later.
///   http://www.pjrc.com/teensy 
///
/// Other platforms are partially supported, such as Generic AVR 8 bit processors, MSP430. 
/// We welcome contributions that will expand the range of supported platforms. 
///
/// \par History
///
/// RadioHead was created in April 2014, substantially based on code from some of our other earlier libraries:
///
/// - RHMesh, RHRouter, RHReliableDatagram and RHDatagram are derived from the RF22 library version 1.39.
/// - RH_RF22 is derived from the RF22 library version 1.39.
/// - RH_RF69 is derived from the RF69 library version 1.2.
/// - RH_ASK is based on the VirtualWire library version 1.26, after significant conversion to C++.
/// - RH_Serial is new.
/// - RH_NRF24 is based on the NRF24 library version 1.12, with some significant changes.
///
/// During this combination and redevelopment, we have tried to retain all the processor dependencies and support from
/// the libraries that were contributed by other people. However not all platforms can be tested by us, so if you
/// find that support from some platform has not been successfully migrated, please feel free to fix it and send us a 
/// patch.
///
/// Users of RHMesh, RHRouter, RHReliableDatagram and RHDatagram in the previous RF22 library will find that their
/// existing code will run mostly without modification. See the RH_RF22 documentation for more details.
///
/// \par Installation
///
/// Install in the usual way: unzip the distribution zip file to the libraries
/// sub-folder of your sketchbook. 
/// The example sketches will be visible in in your Arduino, mpide, maple-ide or whatever.
///
/// \par Compatible Hardware Suppliers
///
/// We have had good experiences with the following suppliers of RadioHead compatible hardware:
///
/// - LittleBird http://littlebirdelectronics.com.au in Australia for all manner of Arduinos and radios.
/// - LowPowerLab http://lowpowerlab.com/moteino in USA for the excellent Moteino and Moteino-USB 
///   boards which include Hope-RF RF69B radios on-board.
/// - Anarduino and HopeRF USA (http://www.hoperfusa.com and http://www.anarduino.com) who have a wide range
///   of HopeRF radios and Arduino integrated modules.
/// - SparkFun https://www.sparkfun.com/ in USA who design and sell a wide range of Arduinos and radio modules.
///
/// \par Donations
///
/// This library is offered under a free GPL license for those who want to use it that way. 
/// We try hard to keep it up to date, fix bugs
/// and to provide free support. If this library has helped you save time or money, please consider donating at
/// http://www.airspayce.com or here:
///
/// \htmlonly <form action="https://www.paypal.com/cgi-bin/webscr" method="post"><input type="hidden" name="cmd" value="_donations" /> <input type="hidden" name="business" value="mikem@airspayce.com" /> <input type="hidden" name="lc" value="AU" /> <input type="hidden" name="item_name" value="Airspayce" /> <input type="hidden" name="item_number" value="RadioHead" /> <input type="hidden" name="currency_code" value="USD" /> <input type="hidden" name="bn" value="PP-DonationsBF:btn_donateCC_LG.gif:NonHosted" /> <input type="image" alt="PayPal — The safer, easier way to pay online." name="submit" src="https://www.paypalobjects.com/en_AU/i/btn/btn_donateCC_LG.gif" /> <img alt="" src="https://www.paypalobjects.com/en_AU/i/scr/pixel.gif" width="1" height="1" border="0" /></form> \endhtmlonly
/// 
/// \par Trademarks
///
/// RadioHead is a trademark of AirSpayce Pty Ltd. The RadioHead mark was first used on April 12 2014 for
/// international trade, and is used only in relation to data communications hardware and software and related services.
/// It is not to be confused with any other similar marks covering other goods and services.
///
/// \par Copyright
///
/// This software is Copyright (C) 2011-2014 Mike McCauley. Use is subject to license
/// conditions. The main licensing options available are GPL V2 or Commercial:
/// 
/// \par Open Source Licensing GPL V2
///
/// This is the appropriate option if you want to share the source code of your
/// application with everyone you distribute it to, and you also want to give them
/// the right to share who uses it. If you wish to use this software under Open
/// Source Licensing, you must contribute all your source code to the open source
/// community in accordance with the GPL Version 2 when your application is
/// distributed. See http://www.gnu.org/copyleft/gpl.html
/// 
/// \par Commercial Licensing
///
/// This is the appropriate option if you are creating proprietary applications
/// and you are not prepared to distribute and share the source code of your
/// application. Contact info@airspayce.com for details.
///
/// \par Revision History
/// \version 1.1 2014-04-14<br>
///              Initial public release
/// \version 1.2 2014-04-23<br>
///              Fixed various typos. <br>
///              Added links to compatible Anarduino products.<br>
///              Added RHNRFSPIDriver, RH_NRF24 classes to support Nordic NRF24 based radios.
/// \version 1.3 2014-04-28<br>
///              Various documentation fixups.<br>
///              RHDatagram::setThisAddress() did not set the local copy of thisAddress. Reported by Steve Childress.<br>
///              Fixed a problem on Teensy with RF22 and RF69, where the interrupt pin needs to be set for input, <br>
///              else pin interrupt doesn't work properly. Reported by Steve Childress and patched by 
///              Adrien van den Bossche. Thanks.<br>
///              Fixed a problem that prevented RF22 honouring setPromiscuous(true). Reported by Steve Childress.<br>
///              Updated documentation to clarify some issues to do with maximum message lengths 
///              reported by Steve Childress.<br>
///              Added support for yield() on systems that support it (currently Arduino 1.5.5 and later)
///              so that spin-loops can suport multitasking. Suggested by Steve Childress.<br>
///              Added RH_RF22::setGpioReversed() so the reversal it can be configured at run-time after
///              radio initialisation. It must now be called _after_ init(). Suggested by Steve Childress.<br>
/// \version 1.4 2014-04-29<br>
///              Fixed further problems with Teensy compatibility for RH_RF22. Tested on Teensy 3.1.
///              The example/rf22_* examples now run out of the box with the wiring connections as documented for Teensy
///              in RH_RF22.<br>
///              Added YIELDs to spin-loops in RHRouter, RHMesh and RHReliableDatagram, RH_NRF24.<br>
///              Tested RH_Serial examples with Teensy 3.1: they now run out of the box.<br>
///              Tested RH_ASK examples with Teensy 3.1: they now run out of the box.<br>
///              Reduced default SPI speed for NRF24 from 8MHz to 1MHz on Teensy, to improve reliability when
///              poor wiring is in use.<br>
///              on some devices such as Teensy.<br>
///              Tested RH_NRF24 examples with Teensy 3.1: they now run out of the box.<br>
/// \version 1.5 2014-04-29<br>
///              Added support for Nordic Semiconductor nRF905 transceiver with RH_NRF905 driver. Also
///              added examples for nRF905 and tested on Teensy 3.1
/// \version 1.6 2014-04-30<br>
///              NRF905 examples were missing
/// \version 1.7 2014-05-03<br>
///              Added support for Arduino Due. Tested with RH_NRF905, RH_Serial, RH_ASK.
///              IMPORTANT CHANGE to interrupt pins on Arduino with RH_RF22 and RH_RF69 constructors:
///              previously, you had to specify the interrupt _number_ not hte interrupt _pin_. Arduinos and Uno32
///              are now consistent with all other platforms: you must specify the interrupt pin number. Default
///              changed to pin 2 (a common choice with RF22 shields).
///              Removed examples/maple/maple_rf22_reliable_datagram_client and 
///              examples/maple/maple_rf22_reliable_datagram_client since the rf22 examples now work out
///              of the box with Flymaple.
///              Removed examples/uno32/uno32_rf22_reliable_datagram_client and 
///              examples/uno32/uno32_rf22_reliable_datagram_client since the rf22 examples now work out
///              of the box with ChipKit Uno32.
/// \version 1.8 2014-05-08 <br>
///              Added support for YIELD in Teensy 2 and 3, suggested by Steve Childress.<br>
///              Documentation updates. Clarify use of headers and Flags<br>
///              Fixed misalignment in RH_RF69 between ModemConfigChoice definitions and the implemented choices
///              which meant you didnt get the choice you thought and GFSK_Rb55555Fd50 hung the transmitter.<br>
///              Preliminary work on Linux simulator.
///
/// \author  Mike McCauley. DO NOT CONTACT THE AUTHOR DIRECTLY. USE THE MAILING LIST GIVEN ABOVE

#ifndef RadioHead_h
#define RadioHead_h

//	Currently supported platforms
#define RH_PLATFORM_ARDUINO      1
#define RH_PLATFORM_MSP430       2
#define RH_PLATFORM_STM32        3
#define RH_PLATFORM_GENERIC_AVR8 4
#define RH_PLATFORM_UNO32        5
#define RH_PLATFORM_SIMULATOR    6

//	Select platform automatically, if possible
#ifndef RH_PLATFORM
 #if defined(MPIDE)
  #define RH_PLATFORM RH_PLATFORM_UNO32
 #elif defined(ARDUINO)
  #define RH_PLATFORM RH_PLATFORM_ARDUINO
 #elif defined(__MSP430G2452__) || defined(__MSP430G2553__)
  #define RH_PLATFORM RH_PLATFORM_MSP430
 #elif defined(MCU_STM32F103RE)
  #define RH_PLATFORM RH_PLATFORM_STM32
 #elif defined(__unix__)
  #define RH_PLATFORM RH_PLATFORM_SIMULATOR
 #else
  #error Platform not defined! 	
 #endif
#endif

#if (RH_PLATFORM == RH_PLATFORM_ARDUINO)
 #if (ARDUINO >= 100)
  #include <Arduino.h>
 #else
  #include <wiring.h>
 #endif

#elif (RH_PLATFORM == RH_PLATFORM_MSP430)// LaunchPad specific
 #include "legacymsp430.h"
 #include "Energia.h"

#elif (RH_PLATFORM == RH_PLATFORM_UNO32)
 #include <WProgram.h>
 #include <string.h>
 #define memcpy_P memcpy

#elif (RH_PLATFORM == RH_PLATFORM_STM32) // Maple etc
 #include <wirish.h>	
 #include <stdint.h>
 #include <string.h>
 // Defines which timer to use on Maple
 #define MAPLE_TIMER 1
 #define PROGMEM
 #define memcpy_P memcpy
 #define Serial SerialUSB
 // Slave select pin
 #ifndef SS
  #define SS 10
 #endif

#elif (RH_PLATFORM == RH_PLATFORM_GENERIC_AVR8) 
 #include <avr/io.h>
 #include <avr/interrupt.h>
 #include <util/delay.h>
 #include <string.h>
 #include <stdbool.h>

#elif (RH_PLATFORM == RH_PLATFORM_SIMULATOR) 
 // Simulate the sketch on Linux
 #include <RHutil/simulator.h>
#else
 #error Platform unknown!
#endif

// This is an attempt to make a portable atomic block
#if (RH_PLATFORM == RH_PLATFORM_ARDUINO)
#if defined(__arm__)
  #include <RHutil/atomic.h>
 #else
  #include <util/atomic.h>
 #endif
 #define ATOMIC_BLOCK_START     ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
 #define ATOMIC_BLOCK_END }
#elif (RH_PLATFORM == RH_PLATFORM_UNO32)
 #include <peripheral/int.h>
 #define ATOMIC_BLOCK_START unsigned int __status = INTDisableInterrupts(); {
 #define ATOMIC_BLOCK_END } INTRestoreInterrupts(__status);
#else 
 // TO BE DONE:
 #define ATOMIC_BLOCK_START
 #define ATOMIC_BLOCK_END
#endif

// Try to be compatible with systems that support yield() and multitasking
// instead of spin-loops
#if (RH_PLATFORM == RH_PLATFORM_ARDUINO && ARDUINO >= 155) || (TEENSYDUINO)
 #define YIELD yield();
#else
 #define YIELD
#endif

// digitalPinToInterrupt is not available prior to Arduino 1.5.6
// See http://arduino.cc/en/Reference/attachInterrupt
#ifndef digitalPinToInterrupt
 #ifndef NOT_AN_INTERRUPT
  #define NOT_AN_INTERRUPT -1
 #endif
#if (RH_PLATFORM == RH_PLATFORM_ARDUINO) && !defined(__arm__)

  #if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  // Arduino Mega, Mega ADK, Mega Pro
  // 2->0, 3->1, 21->2, 20->3, 19->4, 18->5
  #define digitalPinToInterrupt(p) ((p) == 2 ? 0 : ((p) == 3 ? 1 : ((p) >= 18 && (p) <= 21 ? 23 - (p) : NOT_AN_INTERRUPT)))

  #elif defined(__AVR_ATmega32U4__)
  // Leonardo, Yun, Micro, Pro Micro, Flora, Esplora
  // 3->0, 2->1, 0->2, 1->3, 7->4
  #define digitalPinToInterrupt(p) ((p) == 0 ? 2 : ((p) == 1 ? 3 : ((p) == 2 ? 1 : ((p) == 3 ? 0 : ((p) == 7 ? 4 : NOT_AN_INTERRUPT)))))

  #else
  // All other arduino except Due:
  // Serial Arduino, Extreme, NG, BT, Uno, Diecimila, Duemilanove, Nano, Menta, Pro, Mini 04, Fio, LilyPad, Ethernet etc
  // 2->0, 3->1
  #define digitalPinToInterrupt(p)  ((p) == 2 ? 0 : ((p) == 3 ? 1 : NOT_AN_INTERRUPT))

  #endif
 
 #elif (RH_PLATFORM == RH_PLATFORM_UNO32)
  #define digitalPinToInterrupt(p) ((p) == 38 ? 0 : ((p) == 2 ? 1 : ((p) == 7 ? 2 : ((p) == 8 ? 3 : ((p) == 735 ? 4 : NOT_AN_INTERRUPT)))))

 #else
  // Everything else (including Due and Teensy) interrupt number the same as the interrupt pin number
  #define digitalPinToInterrupt(p) (p)
 #endif
#endif


// These defs cause trouble on some versions of Arduino
#undef abs
#undef round
#undef double

// This is the address that indicates a broadcast
#define RH_BROADCAST_ADDRESS 0xff


#endif
