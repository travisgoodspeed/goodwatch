/*! \file radio.h
  \brief RF1A Radio Module Driver
  
*/

//! Set to 1 if the watch has a radio.
extern int has_radio;


//! Read a register from the radio.
uint8_t radio_readreg(uint8_t addr);
//! Write to a register in the radio.
void radio_writereg(uint8_t addr, uint8_t value);

//! Read multiple bytes from a register.
void radio_readburstreg(uint8_t addr,
			uint8_t *buffer, uint8_t count);
//! Write multiple bytes to a register.
void radio_writeburstreg(uint8_t addr,
			 uint8_t *buffer, uint8_t count);

//! Strobe a radio register.
uint8_t radio_strobe(uint8_t strobe);

int radio_getstate();
