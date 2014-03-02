/** RGB service: a service for controlling RGB lights and blinking.
 * This service works on the unicast channel (pipe 1).
 * The following data are supported:
 * red: the amount of red color (0 to 255)
 * green: the amount of green color (0 to 255)
 * blue: the amount of blue color (0 to 255)
 * blinkONtime = time in milliseconds the light must be on (0 to 65535)
 * blinkOFFtime = time in millisecond the light must be off (0 to 65535)
 *
 * Commands can be sent by the server following this format:
 * { "RGB"{ "address": [1,2,3,4], "red": 100, "green": 200 "blue": 0, "blinkONtime": 1000, "blinkOFFtime": 500 }}
 *
 * Author: Dario Salvi (dariosalvi78 at gmail dot com)
 *
 * Licensed under the GPL license http://www.gnu.org/copyleft/gpl.html
 */
#ifndef RGBSERVICE_H_INCLUDED
#define RGBSERVICE_H_INCLUDED
#include <Sensorino.h>
#include <Base.h>

#define RGB_SERVICE 5


/** Packet containing the RGB and blinking information
 */
typedef struct {
    byte red; /**< tred color amount*/
    byte green; /**< green color amount*/
    byte blue; /**< blue color amount*/
    unsigned int blinkONtime; /**< time the light must be ON in milliseconds */
    unsigned int blinkOFFtime; /**< time the light must be OFF in milliseconds */
} RGBPacket;

/** Sends the RGB to the base
 * @param pkt containing RGB information
 * @return true on success
 */
boolean sendRGBtoBase(RGBPacket pkt);

/** Sends the RGB to a sensorino
 * @param address the sensorino address
 * @param pkt containing RGB information
 * @return true on success
 */
boolean sendRGBtoSensorino(byte* address, RGBPacket pkt);

/** Converts bytes back to RGBPacket
 */
RGBPacket parseRGB(byte* data);

/** Sends the data of RGBPacket to the Server
 * uses a JSON message.
 * @param address the address of the sender, 4 bytes
 * @param ints the internals packet
 */
void serverSendRGB(byte* address, RGBPacket rgb);

/** Parses a line coming from the server.
 * Example: { "RGB"{ "address": [1,2,3,4], "red": 100, "green": 200 "blue": 0, "blinkONtime": 1000, "blinkOFFtime": 500 }}
 * @return a RGBPacket with filled parsed data
 */
RGBPacket parseServerRGB(String line);

#endif // RGBSERVICE_H_INCLUDED
