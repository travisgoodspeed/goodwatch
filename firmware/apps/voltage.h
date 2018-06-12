/*! \file voltage.h
  \brief Voltmeter applet.

  This uses the internal reference voltage in the ADC to measure the
  battery voltage.  Later on, we might use this to disable the buzzer
  and the radio when the voltage is too low to support them.

*/


//! Initialize the voltage applet.
void voltage_init();
//! Draw the voltage applet.
void voltage_draw();

