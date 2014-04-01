/** Sensorino energy management library.
 * It uses the features of the ATMega chip to power down the MCU
 * It alsos powerdowns the radio chip
 *
 * Author: Dario Salvi (dariosalvi78 at gmail dot com)
 *
 * Licensed under the GPL license http://www.gnu.org/copyleft/gpl.html
 */
#ifdef __cplusplus
extern "C"
#endif

#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>

#include <Sensorino_Energy.h>


void(* resetf) (void) = 0;

void reset(){
    resetf();
}


//Used to keep track if we have to keep sleeping or not
volatile boolean keepSleeping = true;

//ISR that wakes up the PIC when a pin changes from LOW to HIGH or viceversa
ISR(PCINT0_vect){
    keepSleeping = false;
}
ISR(PCINT1_vect, ISR_ALIASOF(PCINT0_vect));
ISR(PCINT2_vect, ISR_ALIASOF(PCINT0_vect));

//Returns the interrupt mask of the pin
byte pinToInt(byte pin){
  if(pin <=7)
    return PCINT16 + pin;
  else if(pin >=8 && pin <=13)
    return PCINT0 + (pin-8);
  else return PCINT8 + (pin -14);
}

//Returns the interrupt enable mask of the pin
byte pinToIE(byte pin){
  if(pin <=7)
    return PCIE2;
  else if(pin >=8 && pin <=13)
    return PCIE0;
  else return PCIE1;
}

/** Counter of the wake ups produced by the watchdog.
 */
volatile unsigned long totalSleepCounter;

unsigned long getTotalSleepSeconds(){
    return totalSleepCounter;
}

/** seconds to be waited */
int toWaitSeconds = 0;
volatile int sleptSecondsSinceLastWakeUp =0;

/** ISR of the watchdog */
ISR(WDT_vect) {
    totalSleepCounter++;
    sleptSecondsSinceLastWakeUp ++;
    if(( toWaitSeconds >0) && (sleptSecondsSinceLastWakeUp >= toWaitSeconds)){
        //Time to wake up!
        keepSleeping = false;
        sleptSecondsSinceLastWakeUp = 0;
    } else {
        keepSleeping = true;
    }
}


void sleepUntil(int seconds, int* pins, int pinN){
    if(seconds == 0)
        return;

    //make sure we don't get interrupted before we sleep
    noInterrupts ();

    //power down radio:
    nRF24.powerDown();

    //register pin changes
    if(pinN >0) for(int i=0; i< pinN; i++){
        //set pin mask
        if((pins[i]>=0) && (pins[i]<=19)){//pin is considered only if >=0 and <=19
            if(pins[i] <=7)
                PCMSK2 |= (1 << pinToInt(pins[i]));
            else if(pins[i] >=8 && pins[i] <=13)
                PCMSK0 |= (1 << pinToInt(pins[i]));
            else PCMSK1 |= (1 << pinToInt(pins[i]));
            //Register the pin change interrupt
            PCICR |= (1 << pinToIE(pins[i]));
        }
    }

    //Activate the watchdog
    toWaitSeconds = seconds;
    // reset status flag
    MCUSR &= ~(1 << WDRF);
    // enable configuration changes
    WDTCSR |= (1 << WDCE) | (1 << WDE);
    // set the prescalar to 0110 (1 second)
    WDTCSR = (0<<WDP0) | (1<< WDP1) | (1 << WDP2) | (0<<WDP3);
    // enable interrupt mode without reset
    WDTCSR |= _BV(WDIE);

    //Set sleep mode power down:
    //In this mode, the external Oscillator is stopped, while the external interrupts, the 2-
    //wire Serial Interface address watch, and the Watchdog continue operating (if enabled). Only an
    //External Reset, a Watchdog System Reset, a Watchdog Interrupt, a Brown-out Reset, a 2-wire
    //Serial Interface address match, an external level interrupt on INT0 or INT1, or a pin change
    //interrupt can wake up the MCU.
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();

    keepSleeping = true;

    //interrupts allowed now
    interrupts ();
    //Let's sleep !
    while(keepSleeping){
        // turns BOD off, must be done right before sleep
        #ifdef BODS
        MCUCR |= (1<<BODS) | (1<<BODSE);
        MCUCR &= ~(1<<BODSE);
        #endif
        sleep_mode();
    }

    //Here we wake up
    sleep_disable();

    //deactivate watchdog
    wdt_disable();

    //deactivate pins
    if(pinN >0) for(int i=0; i< pinN; i++){
        if((pins[i]>=0) && (pins[i]<=19)){
            PCICR &= ~(1 << pinToIE(pins[i]));
        }
    }

    power_all_enable();
}
