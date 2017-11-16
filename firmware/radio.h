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


//! Read a register from the radio.
uint8_t radio_readreg(uint8_t addr);
//! Write to a register in the radio.
void radio_writereg(uint8_t addr, uint8_t value);
//! Strobe a radio register.
uint8_t radio_strobe(uint8_t strobe);
