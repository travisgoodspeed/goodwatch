/*! \file radio.c
  \brief RF1A Radio Module Driver
  
  This is our neighborly little driver for the RF1A radio module.
  While we try really hard to avoid lasagna-code by over-abstracting
  drivers, the radio needs to be abstracted for good reasons, with
  exceptions to that rule in applications as appropriate.
  
  In general:
  
  1) Not all watches have a radio, so your application ought to
  gracefully fail if XT2 is not present.

  2) The CR2016 battery lasts forever when telling the time, but
  you'll quickly run out of juice if you leave the radio on.  For this
  reason, your app should probably time out after three minutes and
  you should never expect to be receiving in the background.

  3) Remember that transmitting is cheaper than receiving, because a
  transmitter can shut down afterward.
  
  4) The CPU runs at 32kHz by default.  You can speed it up, but at
  the cost of power consumption.
  
*/

#include<msp430.h>

#include "power.h"
#include "radio.h"


//! Set to 1 at boot if the watch has a radio.
int has_radio=0;

//! Called at boot.  Gracefully fails if no radio.
void radio_init(){
  /* First we try to initialize the XT2 crystal.  This will fail if
     the device has no XT2 crystal, so by software we can gracefully
     recognize when a watch has no radio section.
   */
  has_radio=0;
  
}

//! Turns the radio on.  Returns zero on failure.
int radio_on(){
  if(!has_radio){
    return 0; //Failure.
  }

  //Enable high power mode so that LPM3 can be used with an active
  //radio.
  PMMCTL0_H = 0xA5;
  PMMCTL0_L |= PMMHPMRE_L;
  PMMCTL0_H = 0x00;

  //Step up to maximum power.
  power_setvcore(3);
  
  return 1; //Success
}

//! Turns the radio off.
int radio_off(){

  //Disable high-power mode.
  PMMCTL0_H = 0xA5;
  PMMCTL0_L &= ~PMMHPMRE_L;
  PMMCTL0_H = 0x00;

  return 1;
}
