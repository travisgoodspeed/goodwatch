/*! \file radio.h
  \brief RF1A Radio Module Driver
  
*/

//! Set to 1 if the watch has a radio.
extern int has_radio;

//! Called at boot.  Gracefully fails if no radio.
void radio_init();

//! Turns the radio on.  Returns zero on failure.
int radio_on();

//! Turns the radio off.
int radio_off();
