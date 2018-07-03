/*! \file adc.h
  \brief ADC12 Driver for the GoodWatch.
  
  Mostly this uses the internal voltage reference in the ADC to
  measure the battery voltage.  For now, we aren't very interested in
  the temperature or other analog inputs.

*/


//! Grab a voltage reference, in hundreds of a volt.
unsigned int adc_getvcc();
