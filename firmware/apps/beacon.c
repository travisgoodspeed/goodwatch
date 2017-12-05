/*! \file beacon.c
  \brief Beacon Application
  
  Quick little comm demo that sends a callsign as a beacon.

*/

#include<stdio.h>
#include<msp430.h>
#include "api.h"

//Clicker packet length.  (9)
#define LEN 0x3E

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
  IOCFG0,  0x06,   // IOCFG0    GDO0 output pin configuration.
  //Refer to SmartRF Studio User Manual for detailed pseudo register explanation.
  PKTCTRL1,  0x04,   // PKTCTRL1  Packet automation control.
  PKTCTRL0,  0x04,   // PKTCTRL0  Packet automation control.
  ADDR,  0x00,   // ADDR      Device address.
  PKTLEN,  LEN,    // PKTLEN    Packet length.  //0x3e
  0,0
};



//! Enter the Beacon application.
void beacon_init(){
  /* This enters the application.  We use the codeplug frequency.
   */
  if(has_radio){
    printf("Tuning the beacon.\n");
    radio_on();
    radio_strobe(RF_SIDLE);
    radio_writesettings(faradayrf_settings);
    radio_writepower(0x25);
    codeplug_setfreq();
    radio_strobe(RF_SIDLE);
    radio_strobe(RF_SCAL);
    //radio_strobe(RF_SRX);
    //packet_rxon();
  }else{
    app_next();
  }
}

//! Exit the Beacon application.
int beacon_exit(){
  //Stop listening for packets.
  packet_rxoff();
  //Cut the radio off.
  radio_off();
  //Allow the exit.
  return 0;
}

//! Draw the Beacon screen.
void beacon_draw(){
  int state=radio_getstate();

  //lcd_hex(LCDBVCTL);
  if(state==1){
    lcd_string(" beacon ");
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
    
  case 13: //RX Mode
    /* While receiving, the LCD glitches out a bit, so we need to stop
       and resume the reception to allow the charge pump time to
       settle.  This produces a blind-spot in our reception.
     */
    radio_strobe( RF_SIDLE );
    radio_strobe( RF_SRX );
    break;
  }


  switch(getchar()){
  case '7':
    if(radio_getstate()==1){
      //Schedule packet.
      packet_tx((uint8_t*)
		" BEACON BEACON BEACON de " CALLSIGN " " CALLSIGN " " CALLSIGN "\n"
		" BEACON BEACON BEACON de " CALLSIGN " " CALLSIGN " " CALLSIGN "\n"
		" BEACON BEACON BEACON de " CALLSIGN " " CALLSIGN " " CALLSIGN "\n"
		,LEN);
    }
    break;
  case '/':
    /* Receiving packets isn't yet supported.
    if(radio_getstate()==1){
      packet_rxon();
    }
    */
    break;
  case '0':
    packet_rxoff();
  }
}
