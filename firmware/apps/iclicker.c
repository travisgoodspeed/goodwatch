/*! \file iclicker.c
  \brief iClicker Application
  
  This implements the iClicker protocol, as reverse engineered by
  neighbor Ossmann back in the day.  Radio settings were copied from
  his implementation with implied verbal consent, rather than express
  written permission.

*/

#include<stdio.h>
#include<msp430.h>
#include "api.h"

//Clicker packet length.  (9)
#define LEN 9

const uint8_t faradayrf_settings[]={
  FSCTRL1, 0x08,   // FSCTRL1   Frequency synthesizer control.
  FSCTRL0, 0x00,   // FSCTRL0   Frequency synthesizer control.
  FREQ2,  0x23,   // FREQ2     Frequency control word, high byte.
  FREQ1,  0x62,   // FREQ1     Frequency control word, middle byte.
  FREQ0,  0x76,   // FREQ0     Frequency control word, low byte.
  MDMCFG4,  0xCA,   // MDMCFG4   Modem configuration.
  MDMCFG3,  0x83,   // MDMCFG3   Modem configuration.
  MDMCFG2,  0x93,   // MDMCFG2   Modem configuration.
  MDMCFG1,  0x62,   // MDMCFG1   Modem configuration.
  MDMCFG0,  0xF8,   // MDMCFG0   Modem configuration.
  CHANNR,  0x00,   // CHANNR    Channel number.
  DEVIATN,  0x34,   // DEVIATN   Modem deviation setting (when FSK modulation is enabled).
  FREND1,  0x56,   // FREND1    Front end RX configuration.
  FREND0,  0x10,   // FREND0    Front end TX configuration.
  MCSM1, 0x0C,	// MCSM1  (Or this with 2 to keep a carrier after packet.)
  MCSM0,  0x18,   // MCSM0     Main Radio Control State Machine configuration.
  FOCCFG,  0x16,   // FOCCFG    Frequency Offset Compensation Configuration.
  BSCFG,  0x6C,   // BSCFG     Bit synchronization Configuration.
  AGCCTRL2,  0x43,   // AGCCTRL2  AGC control.
  AGCCTRL1,  0x50,   // AGCCTRL1  AGC control.
  AGCCTRL0,  0x91,   // AGCCTRL0  AGC control.
  FSCAL3,  0xE9,   // FSCAL3    Frequency synthesizer calibration.
  FSCAL2,  0x2A,   // FSCAL2    Frequency synthesizer calibration.
  FSCAL1,  0x00,   // FSCAL1    Frequency synthesizer calibration.
  FSCAL0,  0x1F,   // FSCAL0    Frequency synthesizer calibration.
  FSTEST,  0x59,   // FSTEST    Frequency synthesizer calibration.
  TEST2,  0x81,   // TEST2     Various test settings.
  TEST1,  0x35,   // TEST1     Various test settings.
  TEST0,  0x09,   // TEST0     Various test settings.
  FIFOTHR,  0x47,   // FIFOTHR   RXFIFO and TXFIFO thresholds.
  IOCFG2,  0x29,   // IOCFG2    GDO2 output pin configuration.
  IOCFG0,  0x06,   // IOCFG0    GDO0 output pin configuration. Refer to SmartRF® Studio User Manual for detailed pseudo register explanation.
  PKTCTRL1,  0x04,   // PKTCTRL1  Packet automation control.
  PKTCTRL0,  0x04,   // PKTCTRL0  Packet automation control.
  ADDR,  0x00,   // ADDR      Device address.
  PKTLEN,  LEN,    // PKTLEN    Packet length.  //0x3e
  0,0
};

const uint8_t iclicker_settings[]={
  /* IF setting */
  FSCTRL1   , 0x06,
  FSCTRL0   , 0x00,

  /* 905.5 MHz */
  FREQ2     , 0x22,
  FREQ1     , 0xD3,
  FREQ0     , 0xAC,
  CHANNR    , 0x00,

  /* maximum channel bandwidth (812.5 kHz), 152.34 kbaud */
  MDMCFG4   , 0x1C,
  MDMCFG3   , 0x80,

  /* DC blocking enabled, 2-FSK */
  //MDMCFG2   = 0x0l; // 15/16 bit sync
  MDMCFG2   , 0x02, // 16/16 bit sync

  /* no FEC, 2 byte preamble, 250 kHz channel spacing */
  MDMCFG1   , 0x03,
  MDMCFG0   , 0x3B,

  /* 228.5 kHz frequency deviation */
  //DEVIATN   = 0x71;
  /* 253.9 kHz frequency deviation */
  DEVIATN   , 0x72,

  FREND1    , 0x56,   // Front end RX configuration.
  FREND0    , 0x10,   // Front end RX configuration.

  /* automatic frequency calibration */
  MCSM0     , 0x14,
  MCSM1     , 0x30 | 2,   // TXOFF_MODE = IDLE.  | with 2 to keep carrier.

  FSCAL3    , 0xE9,   // Frequency synthesizer calibration.
  FSCAL2    , 0x2A,   // Frequency synthesizer calibration.
  FSCAL1    , 0x00,   // Frequency synthesizer calibration.
  FSCAL0    , 0x1F,   // Frequency synthesizer calibration.
  TEST2     , 0x88,   // Various test settings.
  TEST1     , 0x31,   // Various test settings.
  TEST0     , 0x09,   // high VCO (we're in the upper 800/900 band)
  //  PA_TABLE0 , 0xC0,   // PA output power setting.

  /* no preamble quality check, no address check, no append status */
  //PKTCTRL1  = 0x00;
  //PKTCTRL1  = 0x84;

  /* no preamble quality check, no address check */
  PKTCTRL1 , 0x04,
  /* preamble quality check 2*4=6, address check, append status */
  //PKTCTRL1  , 0x45,

  /* no whitening, no CRC, fixed packet length */
  PKTCTRL0  , 0x00,

  /* packet length in bytes */
  PKTLEN    , LEN,

  SYNC1     , 0xB0,
  SYNC0     , 0xB0,
  ADDR      , 0xB0,

  //Terminate with null *PAIR*.
  0,0
};

/*
 * Channels are designated by the user by entering a two letter button code.
 * This maps the code to a channel number (905.5 MHz + (250 kHz * n)).
 *
 * 'DA' 905.5 MHz, channel 0
 * 'CC' 907.0 MHz, channel 6
 * 'CD' 908.0 MHz, channel 10
 * 'DB' 909.0 MHz, channel 14
 * 'DD' 910.0 MHz, channel 18
 * 'DC' 911.0 MHz, channel 22
 * 'AB' 913.0 MHz, channel 30
 * 'AC' 914.0 MHz, channel 34
 * 'AD' 915.0 MHz, channel 38
 * 'BA' 916.0 MHz, channel 42
 * 'AA' 917.0 MHz, channel 46
 * 'BB' 919.0 MHz, channel 54
 * 'BC' 920.0 MHz, channel 58
 * 'BD' 921.0 MHz, channel 62
 * 'CA' 922.0 MHz, channel 66
 * 'CB' 923.0 MHz, channel 70
 */
//! Clicker channel offsets.
static const uint8_t channel_table[4][4] = { {46, 30, 34, 38},
					     {42, 54, 58, 62},
					     {66, 70, 6,  10},
					     {0,  14, 22, 18} };
/* tune the radio to a particular iClicker channel */
void tune(char *channame) {
  //FIXME bounds checking
  radio_writereg(CHANNR,channel_table[channame[0] - 'A'][channame[1] - 'A']);
}


//! Enter the iClicker application.
void iclicker_init(){
  /* This enters the application.  iClickers run on complicated
     channels, so we tune them directly rather than honoring the
     codeplug.
   */
  if(has_radio){
    printf("Tuning the iClicker.\n");
    radio_on();
    radio_strobe(RF_SIDLE);
    radio_writesettings(iclicker_settings);
    //radio_writesettings(faradayrf_settings);
    radio_writepower(0x25);
    tune("DA");
    radio_strobe(RF_SIDLE);
    radio_strobe(RF_SCAL);
    //radio_strobe(RF_SRX);
    //packet_rxon();
  }else{
    app_next();
  }
}

//! Exit the iClicker application.
int iclicker_exit(){
  //Stop listening for packets.
  packet_rxoff();
  //Cut the radio off.
  radio_off();
  //Allow the exit.
  return 0;
}

//! Draw the iClicker screen.
void iclicker_draw(){
  //  lcd_string("iclicker");
  int state=radio_getstate();

  //lcd_hex(LCDBVCTL);
  if(state==1){
    lcd_string("iclicker");
  }else{
    lcd_number(state);
  }
  

  switch(state){
    /*
  case 17: //RX_OVERFLOW
    printf("RX Overflow.\n");
    radio_strobe(RF_SIDLE);
    break;
  case 22: //TX_OVERFLOW
    printf("TX Overflow.\n");
    radio_strobe(RF_SIDLE);
    break;

  case 1: //IDLE
    printf("Strobing RX.\n");
    radio_strobe(RF_SRX);
    break;
    */
    
  case 13: //RX Mode, LCD will be off.
    radio_strobe( RF_SIDLE );
    radio_strobe( RF_SRX );
    break;
  }

  
  switch(getchar()){
  case '7':
    if(radio_getstate()==1){
      //Schedule packet.
      packet_tx((uint8_t*) "\x01\x01Hello world",LEN);
    }
    break;
  case '/':
    if(radio_getstate()==1){
      packet_rxon();
    }
    break;
  case '0':
    packet_rxoff();
  }
}
