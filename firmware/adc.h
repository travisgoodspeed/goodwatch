/*! \file adc.h
  \brief ADC12 Driver for the GoodWatch.
  
  Mostly this uses the internal voltage reference in the ADC to
  measure the battery voltage.  For now, we aren't very interested in
  the temperature or other analog inputs.

*/


//! Grab a voltage reference, in hundreds of a volt.
unsigned int adc_getvcc();


/* This part is a bit weird.  Basically, we are importing the ADC10
   module that exists in the CC430F6147 but not the CC430F6137.  The
   adc.c library will automatically route calls to either the ADC10 or
   ADC12 unit as appropriate, even though our GCC flags say that we
   are only building for the CC430F6137.
   
   This way, we can generate one single firmware image that works on
   either chip.
 */

#include "adc10.h"


//! Is the ADC of either model active?
#define ADCISACTIVE (ADC12CTL0!=0x3FFF ? (ADC12CTL0&ADC12ON) : (ADC10CTL0&ADC10ON) )

