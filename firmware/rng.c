/*! \file rng.c
  \brief Better random number generator.

  This module implements both the hardware RNG library and the
  rand/srand() functions from the standard library.  We do this
  because for some damned reason srand() calls malloc() in GCC8's
  libc.
*/

#include <msp430.h>
#include "rng.h"

#define RAND_MAX 0x7fffffffL
static unsigned long int next = 1;

int rand(){
  return ((next = next * 1103515245 + 12345) % ((unsigned long int)RAND_MAX + 1));
}

void srand(unsigned int seed){
  next=seed;
}




//SLA338 inspired RNG to be used as a seed for regular PRNG
// see http://www.ti.com/lit/an/slaa338/slaa338.pdf for more details
unsigned int true_rand(void) {
  int i, j;
  unsigned int seed = 0;

  unsigned int UCSCTL1_save = UCSCTL1;// save state and restore later
  unsigned int UCSCTL4_save = UCSCTL4; 
  unsigned int UCSCTL5_save = UCSCTL5;        
  unsigned int TA0CCTL2_save = TA0CCTL2;
  unsigned int TA0CTL_save = TA0CTL;

  TA0CTL = 0x0; // stop timer

  /* setup , according to SLA338:
     Timer_A is setup in capture mode. SMCLK is set to the DCO and
     set as the input clock to Timer_A. ACLK is set to the VLO, 
     which is the trigger for the capture.
  */
  UCSCTL4 = SELA_1 |  SELS_3; // ACLK to VLO, SMCLK to DCO
  // According to cc430f6137.pdf TA0CCTL2 CCI2B is ACLK, therefore CCIS_1
  TA0CCTL2 =  CAP| CM_1 | CCIS_1;
  // capture mode, on rising edge
  TA0CTL = TASSEL_2 | MC_2;        // Timer_A clock source is SMCLK, continuous mode
  /* Generate bits */
  for (i = 0; i < 16; i++) {
    unsigned int ones = 0;
    for (j = 0; j < 5; j++) {
      while(!(TA0CCTL2 & CCIFG)); // wait till interrupt
      TA0CCTL2 &= ~CCIFG; // clear interrupt
      if (1 & TA0CCR2) {// sample LSb 
	ones++;
      }
    }
    seed <<= 1;  
    if (ones >= 3) //majority of 5 samples 
      seed |= 1;
    // even though SLA338 recomends changing dividers and speed to increase 
    // uniformity, tests have shown better results with these two lines disabled
    //UCSCTL1 += 5; // update DCORSEL to change speed
    //UCSCTL5 ^= ((seed & 3) << 8); // update ALCK divider
  }

  // restore everything we've been messing with
  UCSCTL1 = UCSCTL1_save;
  UCSCTL4 = UCSCTL4_save;
  UCSCTL5 = UCSCTL5_save;        
  TA0CCTL2 = TA0CCTL2_save;
  TA0CTL = TA0CTL_save;

  return seed;
}

