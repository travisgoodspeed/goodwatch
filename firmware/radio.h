/*! \file radio.h
  \brief RF1A Radio Module Driver
  
*/

//! Standard radio settings structure from RF Studio.
typedef struct S_RF_SETTINGS {
    unsigned char fsctrl1;   // Frequency synthesizer control.
    unsigned char fsctrl0;   // Frequency synthesizer control.
    unsigned char freq2;     // Frequency control word, high byte.
    unsigned char freq1;     // Frequency control word, middle byte.
    unsigned char freq0;     // Frequency control word, low byte.
    unsigned char mdmcfg4;   // Modem configuration.
    unsigned char mdmcfg3;   // Modem configuration.
    unsigned char mdmcfg2;   // Modem configuration.
    unsigned char mdmcfg1;   // Modem configuration.
    unsigned char mdmcfg0;   // Modem configuration.
    unsigned char channr;    // Channel number.
    unsigned char deviatn;   // Modem deviation setting (when FSK modulation is enabled).
    unsigned char frend1;    // Front end RX configuration.
    unsigned char frend0;    // Front end RX configuration.
    unsigned char mcsm0;     // Main Radio Control State Machine configuration.
    unsigned char foccfg;    // Frequency Offset Compensation Configuration.
    unsigned char bscfg;     // Bit synchronization Configuration.
    unsigned char agcctrl2;  // AGC control.
    unsigned char agcctrl1;  // AGC control.
    unsigned char agcctrl0;  // AGC control.
    unsigned char fscal3;    // Frequency synthesizer calibration.
    unsigned char fscal2;    // Frequency synthesizer calibration.
    unsigned char fscal1;    // Frequency synthesizer calibration.
    unsigned char fscal0;    // Frequency synthesizer calibration.
    unsigned char fstest;    // Frequency synthesizer calibration control
    unsigned char test2;     // Various test settings.
    unsigned char test1;     // Various test settings.
    unsigned char test0;     // Various test settings.
    unsigned char fifothr;   // RXFIFO and TXFIFO thresholds.
    unsigned char iocfg2;    // GDO2 output pin configuration
    unsigned char iocfg0;    // GDO0 output pin configuration
    unsigned char pktctrl1;  // Packet automation control.
    unsigned char pktctrl0;  // Packet automation control.
    unsigned char addr;      // Device address.
    unsigned char pktlen;    // Packet length.
} RF_SETTINGS;


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

//! Writes a radio settings structure.
void radio_writesettings(RF_SETTINGS *pRfSettings);
//! Writes a table of radio settings until the first null pair.
void radio_writesettingstable(const uint8_t *settings);

//! Writes one value to the power table.
void radio_writepower(uint8_t value);

//! Read the RSSI.
int radio_getrssi();
//! Read the radio MARC state.
int radio_getstate();
