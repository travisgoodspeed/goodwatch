/*! \file ref.c
  \brief Power reference module for the CC430F6137 and CC430F6147.
  
  Previously, we used the built-in power request features of the ADC12
  in the CC430F6137, but this is bad for two reasons.  First, the
  ADC10 in the '6147 lacks power requests, and second, there are more
  efficient power modes that might reduce the current consumption of
  our LCD charge pump.
  
*/

#include <msp430.h>
#include <stdio.h>
#include "api.h"


//! Turn the reference on.
void ref_on(){
  REFCTL0 |= REFON;
}

//! Turn the reference off.
void ref_off(){
  REFCTL0 &= ~REFON;
}

//! Switch the the static mode, which is power hungry but more reliable.
void ref_staticmode(){
  REFCTL0 |= BGMODE;
}

//! Switch to sample mode, which is more efficient.
void ref_sampledmode(){
  REFCTL0 &= ~BGMODE;
}


//! Activates and configures the module, but does not turn on reference.
void ref_init(){
  //Setting the master bit disables legacy mode.
  REFCTL0|=REFMSTR;

  //Wait for register to not be busy before modifying voltage.
  while(REFCTL0 & REFGENBUSY);
  
  //2.5V reference.
  ref_sampledmode();
  REFCTL0|=REFVSEL_2;
}


//! Print the REF status.
void ref_status(){
  printf("REFCTL0=0x%04x\n", REFCTL0);
}
