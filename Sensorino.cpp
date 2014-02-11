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
#ifdef __cplusplus
extern "C"
#endif

#include <Sensorino.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>


void configure(byte chipEnablePin, byte chipSelectPin, byte irqPin, byte myAdd[]) {
    NRF24::configure(chipEnablePin, chipSelectPin, irqPin);
    thisAddress[0] = myAdd[0];
    thisAddress[1] = myAdd[1];
    thisAddress[2] = myAdd[2];
    thisAddress[3] = myAdd[3];
}

boolean start(){
    //Set sleep mode power down:
    //In this mode, the external Oscillator is stopped, while the external interrupts, the 2-
    //wire Serial Interface address watch, and the Watchdog continue operating (if enabled). Only an
    //External Reset, a Watchdog System Reset, a Watchdog Interrupt, a Brown-out Reset, a 2-wire
    //Serial Interface address match, an external level interrupt on INT0 or INT1, or a pin change
    //interrupt can wake up the MCU.
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();

    //Init the nrf24
    nRF24.init();
    if(!nRF24.setChannel(RF_CHANNEL)) return false;
    //set dynamic payload size
    if(!nRF24.setPayloadSize(0, 0)) return false;
    if(!nRF24.setPayloadSize(1, 0)) return false;
    //Set address size to 4
    if(!nRF24.setAddressSize(NRF24::NRF24AddressSize4Bytes)) return false;
    //Set CRC to 2 bytes
    if(!nRF24.setCRC(NRF24::NRF24CRC2Bytes)) return false;
    //Set 2 Mbps, maximum power
    if(!nRF24.setRF(NRF24::NRF24DataRate2Mbps, NRF24::NRF24TransmitPower0dBm)) return false;
    //Configure pipes
    if(!nRF24.setPipeAddress(0, broadCastAddress)) return false;
    if(!nRF24.enablePipe(0)) return false;
    if(!nRF24.setAutoAck(0, false)) return false;
    if(!nRF24.setPipeAddress(1, thisAddress)) return false;
    if(!nRF24.enablePipe(1)) return false;
    if(!nRF24.setAutoAck(1, true)) return false;
    //Configure retries
    if(!nRF24.setTXRetries(3, 3)) return false;
    return true;
}

//ISR of the pin used for wakeup
void wakeUpOnPinISR(){
    //detach the interrupt until we need it again
    if(wakeUpPin != -1){
        if(wakeUpPin == 2) detachInterrupt(0);
        else detachInterrupt(1);
    }
}

void wakeUpOnPinChange(byte pin){
    wakeUpPin = pin;
    pinMode(pin, INPUT);
    //Activate interrupts, in case they aren't
    interrupts();
}

void wakeUpPeriodically(){
    periodicWakeUps = 0;
}

//ISR of the watchdog
ISR( WDT_vect ) {
    periodicWakeUps++;
    if(periodicWakeUps <0)
        periodicWakeUps = 0;
    wdt_disable();
}


void sleep(){
    //power down radio:
    nRF24.powerDown();

    //Register the pin change interrupt
    if(wakeUpPin != -1){
        if(wakeUpPin == 2) attachInterrupt(0, wakeUpOnPinISR, CHANGE);
        else attachInterrupt(1, wakeUpOnPinISR, CHANGE);
        delay(100);
    }
    //And/or activate the watchdog
    if(periodicWakeUps != -1){
        // reset status flag
        MCUSR &= ~(1 << WDRF);
        // enable configuration changes
        WDTCSR |= (1 << WDCE) | (1 << WDE);
        // set the prescalar to 9 = 1001
        WDTCSR = (1<< WDP0) | (1 << WDP3);
        // enable interrupt mode without reset
        WDTCSR |= _BV(WDIE);
    }

    //Let's sleep !
    sleep_mode();

    //Here we wake up
    sleep_disable();
    //watchdog and pin interrupt should be disactivated already

    power_all_enable();
}

void composeBasePacket(byte* buffer, unsigned int service, byte* data, int len){
    int totlen = 6 + len;
    buffer[0] = thisAddress[0];
    buffer[1] = thisAddress[1];
    buffer[2] = thisAddress[2];
    buffer[3] = thisAddress[3];
    buffer[4] = service & 0xFF ;
    buffer[5] = (service >> 8) & 0xFF;
    for(int i=0; i<len;i++){
     buffer[i+6] = data[i];
    }
}

void decomposeBasePacket(byte* packet, int totlen, byte* sender,
                                    unsigned int* service, byte* data, int* len){
    sender[0] = packet[0];
    sender[1] = packet[1];
    sender[2] = packet[2];
    sender[3] = packet[3];
    *service = (unsigned int)(packet[5] <<8) + (unsigned int)packet[4];
    *len = totlen -6;
    for(int i=0; i<*len; i++){
     data[i] = packet[i+6];
    }
}

boolean sendToBase(unsigned int service, byte* data, int len){
    if(!nRF24.setTransmitAddress(baseAddress))
        return false;
    byte pkt[6+len];
    composeBasePacket(pkt, service, data, len);
    return nRF24.send(pkt, 6+len, false);
}

boolean sendToBroadcast(unsigned int service, byte* data, int len){
    if(!nRF24.setTransmitAddress(broadCastAddress))
        return false;
    byte pkt[6+len];
    composeBasePacket(pkt, service, data, len);
    return nRF24.send(pkt, 6+len, true);
}

boolean receive(unsigned int timeout, boolean* broadcast, byte* sender,
                           unsigned int* service, byte* data, int* len){
    byte buffer[NRF24_MAX_MESSAGE_LEN];
    byte totlen;
    byte pipe;
    if(nRF24.waitAvailableTimeout(timeout)){
            if(nRF24.recv(&pipe, buffer, &totlen)){
                *broadcast = (pipe == 0);
                decomposeBasePacket(buffer, totlen, sender, service, data, len);
            return true;
        }
    }
    return false;
}
