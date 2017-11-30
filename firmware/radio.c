/*! \file radio.c
  \brief RF1A Radio Module Driver
  
  This is our neighborly little driver for the RF1A radio module.
  While we try really hard to avoid lasagna-code by over-abstracting
  drivers, the radio needs to be abstracted for good reasons, with
  exceptions to that rule in applications as appropriate.
  
  In general:
  
  1) Not all watches have a radio, so your application ought to
  gracefully fail if XT2 is not present.

  2) The CR2016 battery lasts forever when telling the time, but
  you'll quickly run out of juice if you leave the radio on.  For this
  reason, your app should probably time out after three minutes and
  you should never expect to be receiving in the background.

  3) Remember that transmitting is cheaper than receiving, because a
  transmitter can shut down afterward.
  
  4) The CPU runs at 32kHz by default.  You can speed it up, but at
  the cost of power consumption.
  
*/

#include<stdio.h>
#include<stdint.h>
#include<msp430.h>

#include "power.h"
#include "radio.h"
#include "configdefault.h"


//! Cleared to zero at the first radio failure.
int has_radio=1;

//! Sets the radio frequency.
void radio_setfreq(float freq){
  float freqMult = (0x10000 / 1000000.0) / 26;
  uint32_t num = freq * freqMult;
  
  //Store the frequency.
  radio_writereg(FREQ2, (num >> 16) & 0xFF);
  radio_writereg(FREQ1, (num >> 8) & 0xFF);
  radio_writereg(FREQ0, num & 0xFF);

  //Strobe a calibration to make it count.
  radio_strobe(RF_SCAL);
  //Wait for it to take effect.
  while(radio_getstate()!=1);
}

//! Sets the raw radio frequency registers.
void radio_setrawfreq(uint8_t freq2, uint8_t freq1, uint8_t freq0){
  //Store the frequency.
  radio_writereg(FREQ2, freq2);
  radio_writereg(FREQ1, freq1);
  radio_writereg(FREQ0, freq0);

  //Strobe a calibration to make it count.
  radio_strobe(RF_SCAL);
  //Wait for it to take effect.
  while(radio_getstate()!=1);
}

//! Gets the radio frequency.
uint32_t radio_getfreq(){
  static uint32_t oldhex=0, oldnum=0;
  uint32_t hex=
    0xFF0000l & (((uint32_t) radio_readreg(FREQ2))<<16);
  hex|= (0xFF00l & (radio_readreg(FREQ1)<<8));
  hex|= (0xFFl & (radio_readreg(FREQ0)));

  //Return the old value if it hasn't changed.
  if(oldhex==hex)
    return oldnum;

  //Otherwise calculate the new value and return it.
  oldhex=hex;
  oldnum=hex*396.728515625;
  return oldnum;
}


//! Called at boot.  Gracefully fails if no radio.
void radio_init(){
  /* If the radio components are missing, the AVCC_RF lines will be
     unconnected and the radio will immediately have a low voltage error.
  */
  radio_on();


  /* We can't check RF1AIFERR&1 to tell whether the radio circuit is
     powered, because Errata RF1A6 makes that bit useless.  Instead,
     we run a radio strobe and look for its reply.
   */
  radio_strobe(RF_SCAL);
  printf("This watch has %s radio.\n",
	 has_radio?"a":"no");

  /*
  //Chirp a bit if we have a radio.
  if(has_radio){
    //Load the default frequency.
    radio_writesettings(NULL);
    //radio_writepower(0x2D); //-6 dBm
    radio_writepower(0x25); //-12 dBm
    radio_setfreq(434000000);
    radio_strobe(RF_SCAL);
      
    
    //Morse test.
    
    //while(1)
    //radio_morse("-.- -.- ....- ...- --..    ");
    
  }
  
  radio_off();
  radio_on();
  radio_off();
  */
  radio_off();
}

//! Turns the radio on.  Returns zero on failure.
int radio_on(){
  if(!has_radio){
    return 0; //Failure.
  }

  //Enable high power mode so that LPM3 can be used with an active
  //radio.
  PMMCTL0_H = 0xA5;
  PMMCTL0_L |= PMMHPMRE_L;
  PMMCTL0_H = 0x00;

  //Step up the core voltage a bit.
  while(!power_setvcore(COREVOLTAGE)){
    printf("Failed to set vcore.\n");
    __delay_cycles(850);
  }
  __delay_cycles(850);

  //Strobe the radio to reset it.
  radio_resetcore();

  
 
  
  return 1; //Success
}

//! Restarts the radio.
void radio_resetcore(){
  radio_strobe(RF_SRES);
  radio_strobe(RF_SNOP);
}


//! Turns the radio off.
int radio_off(){
  //Cut the radio's oscillator.
  radio_strobe(RF_SIDLE);
  radio_strobe(RF_SXOFF);

  
  /* We really ought to lower the
     core voltage, but seems that it
     can never come back up.
  */
  //Drop the voltage first.
  power_setvcore(0);
  __delay_cycles(850);
  

  //Then disable high-power mode.
  PMMCTL0_H = 0xA5;
  PMMCTL0_L &= ~PMMHPMRE_L;
  PMMCTL0_H = 0x00;
  
  return 1;
}


//! Read a register from the radio.
uint8_t radio_readreg(uint8_t addr){
  // Check for valid configuration register address, 0x3E refers to PATABLE 
  if ((addr <= 0x2E) || (addr == 0x3E))
    // Send address + Instruction + 1 dummy byte (auto-read)
    RF1AINSTR1B = (addr | RF_SNGLREGRD);    
  else
    // Send address + Instruction + 1 dummy byte (auto-read)
    RF1AINSTR1B = (addr | RF_STATREGRD);    
  
  while (!(RF1AIFCTL1 & RFDOUTIFG) );
  
  //Reading the data clears the interrupt flag.
  return RF1ADOUTB;
}

//! Write to a register in the radio.
void radio_writereg(uint8_t addr, uint8_t value){
  // Wait until the radio is ready.
  while (!(RF1AIFCTL1 & RFINSTRIFG));
  
  // Send the address and instruction.
  RF1AINSTRB = (addr | RF_SNGLREGWR);
  // And the value.
  RF1ADINB = value;
}

// Chipcon
// Product = CC430Fx13x
// Chip version = C   (PG 0.7)
// Crystal accuracy = 10 ppm
// X-tal frequency = 26 MHz
// RF output power = 0 dBm
// RX filterbandwidth = 101.562500 kHz
// Deviation = 19 kHz
// Datarate = 38.383484 kBaud
// Modulation = (1) GFSK
// Manchester enable = (0) Manchester disabled
// RF Frequency = 914.999969 MHz
// Channel spacing = 199.951172 kHz
// Channel number = 0
// Optimization = -
// Sync mode = (3) 30/32 sync word bits detected
// Format of RX/TX data = (0) Normal mode, use FIFOs for RX and TX
// CRC operation = (1) CRC calculation in TX and CRC check in RX enabled
// Forward Error Correction = 
// Length configuration = (0) Fixed packet length, packet length configured by PKTLEN
// Packetlength = 61
// Preamble count = (2)  4 bytes
// Append status = 1
// Address check = (0) No address check
// FIFO autoflush = 0
// Device address = 0
// GDO0 signal selection = ( 6) Asserts when sync word has been sent / received, and de-asserts at the end of the packet
// GDO2 signal selection = (41) RF_RDY
RF_SETTINGS rfSettingsDefault = {
    0x08,   // FSCTRL1   Frequency synthesizer control.
    0x00,   // FSCTRL0   Frequency synthesizer control.
    0x23,   // FREQ2     Frequency control word, high byte.
    0x31,   // FREQ1     Frequency control word, middle byte.
    0x3B,   // FREQ0     Frequency control word, low byte.
    0xCA,   // MDMCFG4   Modem configuration.
    0x83,   // MDMCFG3   Modem configuration.
    0x93,   // MDMCFG2   Modem configuration.
    0x22,   // MDMCFG1   Modem configuration.
    0xF8,   // MDMCFG0   Modem configuration.
    0x00,   // CHANNR    Channel number.
    0x34,   // DEVIATN   Modem deviation setting (when FSK modulation is enabled).
    0x56,   // FREND1    Front end RX configuration.
    0x10,   // FREND0    Front end TX configuration.
    0x18,   // MCSM0     Main Radio Control State Machine configuration.
    0x16,   // FOCCFG    Frequency Offset Compensation Configuration.
    0x6C,   // BSCFG     Bit synchronization Configuration.
    0x43,   // AGCCTRL2  AGC control.
    0x40,   // AGCCTRL1  AGC control.
    0x91,   // AGCCTRL0  AGC control.
    0xE9,   // FSCAL3    Frequency synthesizer calibration.
    0x2A,   // FSCAL2    Frequency synthesizer calibration.
    0x00,   // FSCAL1    Frequency synthesizer calibration.
    0x1F,   // FSCAL0    Frequency synthesizer calibration.
    0x59,   // FSTEST    Frequency synthesizer calibration.
    0x81,   // TEST2     Various test settings.
    0x35,   // TEST1     Various test settings.
    0x09,   // TEST0     Various test settings.
    0x47,   // FIFOTHR   RXFIFO and TXFIFO thresholds.
    0x29,   // IOCFG2    GDO2 output pin configuration.
    0x06,   // IOCFG0    GDO0 output pin configuration. Refer to SmartRF? Studio User Manual for detailed pseudo register explanation.
    0x04,   // PKTCTRL1  Packet automation control.
    0x04,   // PKTCTRL0  Packet automation control.
    0x00,   // ADDR      Device address.
    0x64    // PKTLEN    Packet length.
};


//! Writes a radio settings structure.
void radio_writesettings(RF_SETTINGS *pRfSettings) {
  if(!pRfSettings)
    pRfSettings= &rfSettingsDefault;
  
  radio_writereg(FSCTRL1,  pRfSettings->fsctrl1);
  radio_writereg(FSCTRL0,  pRfSettings->fsctrl0);
  radio_writereg(FREQ2,    pRfSettings->freq2);
  radio_writereg(FREQ1,    pRfSettings->freq1);
  radio_writereg(FREQ0,    pRfSettings->freq0);
  radio_writereg(MDMCFG4,  pRfSettings->mdmcfg4);
  radio_writereg(MDMCFG3,  pRfSettings->mdmcfg3);
  radio_writereg(MDMCFG2,  pRfSettings->mdmcfg2);
  radio_writereg(MDMCFG1,  pRfSettings->mdmcfg1);
  radio_writereg(MDMCFG0,  pRfSettings->mdmcfg0);
  radio_writereg(CHANNR,   pRfSettings->channr);
  radio_writereg(DEVIATN,  pRfSettings->deviatn);
  radio_writereg(FREND1,   pRfSettings->frend1);
  radio_writereg(FREND0,   pRfSettings->frend0);
  radio_writereg(MCSM0 ,   pRfSettings->mcsm0);
  radio_writereg(FOCCFG,   pRfSettings->foccfg);
  radio_writereg(BSCFG,    pRfSettings->bscfg);
  radio_writereg(AGCCTRL2, pRfSettings->agcctrl2);
  radio_writereg(AGCCTRL1, pRfSettings->agcctrl1);
  radio_writereg(AGCCTRL0, pRfSettings->agcctrl0);
  radio_writereg(FSCAL3,   pRfSettings->fscal3);
  radio_writereg(FSCAL2,   pRfSettings->fscal2);
  radio_writereg(FSCAL1,   pRfSettings->fscal1);
  radio_writereg(FSCAL0,   pRfSettings->fscal0);
  radio_writereg(FSTEST,   pRfSettings->fstest);
  radio_writereg(TEST2,    pRfSettings->test2);
  radio_writereg(TEST1,    pRfSettings->test1);
  radio_writereg(TEST0,    pRfSettings->test0);
  radio_writereg(FIFOTHR,  pRfSettings->fifothr);
  radio_writereg(IOCFG2,   pRfSettings->iocfg2);
  radio_writereg(IOCFG0,   pRfSettings->iocfg0);
  radio_writereg(PKTCTRL1, pRfSettings->pktctrl1);
  radio_writereg(PKTCTRL0, pRfSettings->pktctrl0);
  radio_writereg(ADDR,     pRfSettings->addr);
  radio_writereg(PKTLEN,   pRfSettings->pktlen);
}


//! Strobe a radio register.
uint8_t radio_strobe(uint8_t strobe){
  uint8_t  statusByte = 0;
  uint16_t count=0;
  uint16_t gdo_state;
  
  // Check for valid strobe command 
  if((strobe == 0xBD) || ((strobe >= RF_SRES) && (strobe <= RF_SNOP))){
    // Clear the Status read flag 
    RF1AIFCTL1 &= ~(RFSTATIFG);    
    
    // Wait for radio to be ready for next instruction
    while( !(RF1AIFCTL1 & RFINSTRIFG));
    
    
    // Write the strobe instruction
    if ((strobe > RF_SRES) && (strobe < RF_SNOP)){
      gdo_state = radio_readreg(IOCFG2);    // buffer IOCFG2 state
      radio_writereg(IOCFG2, 0x29);         // chip-ready to GDO2
      
      RF1AINSTRB = strobe; 
      if ( (RF1AIN&0x04)== 0x04 ) {         // chip at sleep mode
        if ( (strobe == RF_SXOFF) || (strobe == RF_SPWD) || (strobe == RF_SWOR) ) {
	  
	}else{
	  /* We'll get stuck in an infinite loop here if the radio
	     crystal isn't available.
	   */
          while ((RF1AIN&0x04)== 0x04){
	    if(count++>1000){
	      printf("Timeout in radio_strobe.  Broken XT2?\n");
	      has_radio=0;
	      return 0xFF;
	    }
	  }
	  
          // Delay for ~810usec at 1.05MHz CPU clock, see erratum RF1A7
          __delay_cycles(850);
        }
      }
      radio_writereg(IOCFG2, gdo_state);    // restore IOCFG2 setting
      
      while( !(RF1AIFCTL1 & RFSTATIFG) );
    }else{ //Active mode
      RF1AINSTRB = strobe;
    }
    statusByte = RF1ASTATB;
  }
  return statusByte;
}

//! Writes one value to the power table.
void radio_writepower(uint8_t value) {
  while( !(RF1AIFCTL1 & RFINSTRIFG));
  RF1AINSTRW = 0x3E00 + value;              // PA Table single write
  
  while( !(RF1AIFCTL1 & RFINSTRIFG));
  RF1AINSTRB = RF_SNOP;                     // reset PA_Table pointer
}

//! Read the RSSI.
int radio_getrssi(){
  int rssi;
  
  //Enter RX mode to get the value if we aren't already there.
  radio_strobe(RF_SRX);
  //Need a delay before the value becomes valid.
  __delay_cycles(400);
  
  //Idle when we're done.
  radio_strobe(RF_SIDLE);
  
  //Grab the new value.
  rssi=radio_readreg(RSSI)^0x80;
  
  return rssi;
}

//! Read the radio MARC state.
int radio_getstate(){
  int state;
  state=radio_readreg(MARCSTATE);
  return state;
}
