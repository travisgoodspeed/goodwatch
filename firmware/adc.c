/*! \file adc.c
  \brief ADC12 and ADC10 Driver for the GoodWatch.
  
  Mostly this uses the internal voltage reference in the ADC to
  measure the battery voltage.  For now, we aren't very interested in
  the temperature or other analog inputs.

*/

#include <msp430.h>
#include <stdio.h>
#include "api.h"

//! Grab a voltage reference in a 6137, in hundreds of a volt.
static unsigned int adc12_getvcc(){
  unsigned int vcc;

  /* Here is we power up the ADC and configure it for use.  We really
     ought to shut it down at the end, but for now, we trust that the
     auto-powerdown works.
     
     We should also move this function into its own module, so that
     the voltage can be checked at other times.
   */
  ADC12CTL0 = ADC12ON+ADC12SHT0_2; // Turn on and set up ADC12
  ADC12CTL1 = ADC12SHP;                     // Use sampling timer
  ADC12MCTL0 = ADC12SREF_1 | ADC12INCH_11;  // Vr+=Vref+, voltage/2 input.

  // Enable conversions
  ADC12CTL0 |= ADC12ENC;                    

  //Start the conversion and fetch the result when ready.
  ADC12CTL0 |= ADC12SC;
  while ((ADC12IFG & BIT0)==0);


  /* 
     So by this stage, the result is in ADC12MEM0.  For example, if it
     is 0xb2a, we can find the count by realizing that the maximum
     range (4095) is equal to 2.5 volts, while we are measuring half of
     VCC.

     So 0xb2a*2.0 / 4095 * 2.5 == 3.49, our battery voltage.
     Simplified, we can get hundreds of a volt by mulitplying
     ADC12MEM0 with 0.1221.
   */
  vcc= (int) ADC12MEM0*0.1221001221001221;


  /* The ADC is supposed to turn itself off automatically, but our
     watch will die an early death if we don't shut it down, so we do
     it manually.
     
     As an added tricky bit, we have to stop the current conversion,
     then power down the controller, or it will be stuck running.  You
     can't just zero the registers.
  */
  
  ADC12CTL0 &= ~ADC12ENC; //Disable conversion
  ADC12CTL0&=~ADC12ON; //Power off the controller.
  //Then we zero any remaining flags.
  ADC12CTL0=0;
  ADC12CTL1=0;
  ADC12MCTL0=0;

  return vcc;
}


//! Grab a voltage reference from a CC430C6147, in hundreds of a volt.
static unsigned int adc10_getvcc(){
  unsigned int vcc;

  /* Here is we power up the ADC and configure it for use.  We really
     ought to shut it down at the end, but for now, we trust that the
     auto-powerdown works.
     
     We should also move this function into its own module, so that
     the voltage can be checked at other times.
   */
  ADC10CTL0 = ADC10ON+ADC10SHT2;            // Turn on and set up ADC10
  ADC10CTL1 = ADC10SHP;                     // Use sampling timer
  ADC10MCTL0 = ADC10SREF_1 | ADC10INCH_11;  // Vr+=Vref+, voltage/2 input.

  // Enable conversions
  ADC10CTL0 |= ADC10ENC;                    

  //Start the conversion and fetch the result when ready.
  ADC10CTL0 |= ADC10SC;
  while ((ADC10IFG & BIT0)==0);

  /* 
     The range is a quarter that of the ADC12 module in the '6137, so
     we can get hundreds of a volt by mulitplying ADC10MEM0 with
     0.4884004884004884.
   */
  vcc= (int) ADC10MEM0*0.4884004884004884;

  /* The ADC is supposed to turn itself off automatically, but our
     watch will die an early death if we don't shut it down, so we do
     it manually.
     
     As an added tricky bit, we have to stop the current conversion,
     then power down the controller, or it will be stuck running.  You
     can't just zero the registers.
  */
  
  ADC10CTL0 &= ~ADC10ENC; //Disable conversion
  ADC10CTL0 &= ~ADC10ON;  //Power off the controller.
  //Then we zero any remaining flags.
  ADC10CTL0=0;
  ADC10CTL1=0;
  ADC10MCTL0=0;

  //printf("VCC sampled as %d\n", vcc);
  
  return vcc;
}


//! Grab a voltage reference, in hundreds of a volt.
unsigned int adc_getvcc(){
  switch(DEVICEID){
  case DEVICEID6137:
    return adc12_getvcc();
    break;
  case DEVICEID6147:
    return adc10_getvcc();
    break;
  default:
    printf("Unknown Chip ID, returning 0V.\n");
    return 0;
    break;
  }
}
