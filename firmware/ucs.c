/*! \file ucs.c
  \brief Clocking functions.
  
  This module implements a minimal driver for the Unified Clock System
  of the CC430F6137 and related devices.  In general, we try to run in
  slow mode whenever possible, jumping to fast mode only during
  diagnostics or if required (briefly) for a radio exchange.  At all
  other times, we run the CPU at 32kHz for power efficiency.
  
*/

#include <msp430.h>
#include "api.h"

//! Fast mode.
void ucs_fast(){
  UCSCTL4 = SELM_3 + SELS_0 + SELA_0;  //XT1 for ACLK and SMCLK, MCLK from DCO.
}

//! Slow mode.
void ucs_slow(){
  #ifdef TURBOMODE
  ucs_fast();
  #else
  UCSCTL4 = SELM_0 + SELS_0 + SELA_0;    //XT1 for everything; very slow CPU.
  #endif
}

//! Initialize the XT1 crystal, and stabilize it.
void ucs_init(){
  P5SEL |= BIT0 + BIT1;                     // Select XT1
  UCSCTL6 |= XCAP_1;                        // Internal load cap

  // Loop until XT1 & DCO stabilizes
  do{
    UCSCTL7 &= ~(XT1LFOFFG + DCOFFG);
                                            // Clear LFXT1,DCO fault flags
    SFRIFG1 &= ~OFIFG;                      // Clear fault flags
  }while (SFRIFG1 & OFIFG);                 // Test oscillator fault flag

  UCSCTL6 &= ~(XT1DRIVE_3);                 // Xtal is now stable, reduce drive
                                            // strength
  //See page 125 of the family guide.
  ucs_fast();
}
