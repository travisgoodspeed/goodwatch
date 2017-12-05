/*! \file radio.h
  \brief RF1A Radio Module Driver
  
*/

//! Set to 1 if the watch has a radio.
extern int has_radio;

//! Called at boot.  Gracefully fails if no radio.
void radio_init();

//! Turns the radio on.  Returns zero on failure.
void radio_on();

//! Sets the radio frequency.
void radio_setfreq(float freq);
//! Sets the raw radio frequency registers.
void radio_setrawfreq(uint8_t freq2, uint8_t freq1, uint8_t freq0);
//! Gets the radio frequency.
uint32_t radio_getfreq();

//! Restarts the radio.  (Call this after radio_on().)
void radio_resetcore();

//! Turns the radio off.
void radio_off();


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

//! Writes a table of radio settings until the first null pair.
void radio_writesettings(const uint8_t *settings);

//! Writes one value to the power table.
void radio_writepower(uint8_t value);

//! Read the RSSI.
int radio_getrssi();
//! Read the radio MARC state.
int radio_getstate();
