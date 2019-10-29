/*! \file beacon.c
  \brief Beacon Application
  
  Quick little comm demo that sends a callsign as a beacon.

*/

#include<stdio.h>
#include<string.h>
#include<msp430.h>
#include "api.h"

//Fixed packet length is easier on our packet library.
#define LEN 32

/* Settings for the GoodWatch, similar to 1.2kbaud SimpliciTI.
 */
static const uint8_t goodwatch_settings[]={
  IOCFG0,0x06,  //GDO0 Output Configuration
  FIFOTHR,0x47, //RX FIFO and TX FIFO Thresholds
  PKTCTRL1, 0x04, //No address check.
  //PKTCTRL0, 0x05,//Packet Automation Control, variable length.
  PKTCTRL0, 0x04, //Packet automation control, fixed length.
  FSCTRL1,0x06, //Frequency Synthesizer Control
  FREND0,    0x11,   // FREND0    Front end TX configuration, use PA_TABLE[1]
  FREQ2,0x21,   //Frequency Control Word, High Byte
  FREQ1,0x62,   //Frequency Control Word, Middle Byte
  FREQ0,0x76,   //Frequency Control Word, Low Byte
  MDMCFG4,0xF5, //Modem Configuration
  MDMCFG3,0x83, //Modem Configuration
  MDMCFG2,0x13, //Modem Configuration
  DEVIATN,0x15, //Modem Deviation Setting
  MCSM0,0x10,   //Main Radio Control State Machine Configuration
  FOCCFG,0x16,  //Frequency Offset Compensation Configuration
  WORCTRL,0xFB, //Wake On Radio Control
  FSCAL3,0xE9,  //Frequency Synthesizer Calibration
  FSCAL2,0x2A,  //Frequency Synthesizer Calibration
  FSCAL1,0x00,  //Frequency Synthesizer Calibration
  FSCAL0,0x1F,  //Frequency Synthesizer Calibration
  TEST2,0x81,   //Various Test Settings
  TEST1,0x35,   //Various Test Settings
  TEST0,0x09,   //Various Test Settings
  ADDR,  0x00,   // ADDR      Device address.
  MCSM1, 0x30,   //MCSM1, return to IDLE after packet.  Or with 2 for TX carrier test.
  MCSM0,  0x18,   // MCSM0     Main Radio Control State Machine configuration.
  IOCFG2,  0x29,   // IOCFG2    GDO2 output pin configuration.
  IOCFG0,  0x06,   // IOCFG0    GDO0 output pin configuration.
  PKTLEN,  LEN,    // PKTLEN    Packet length.
  0,0
};


static char lastpacket[]="IDLE    ";

//! Handle an incoming packet.
void beacon_packetrx(uint8_t *packet, int len){
  /* Quickly change the idle screen to the first eight bytes of the
     packet.  We'll do something fancier later.
   */
  memcpy(lastpacket,packet,8);
}


//! Enter the Beacon application.
void beacon_init(){
  /* This enters the application.  We use the codeplug frequency.
   */
  if(has_radio){
    printf("Tuning the beacon.\n");
    radio_on();
    radio_writesettings(goodwatch_settings);
    radio_writepower(0x25);
    codeplug_setfreq();
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
    /* Draw the last incoming packet on the screen. */
    lcd_string(lastpacket);
  }else{
    lcd_number(state);
  }
  

  switch(state){

  case 17: //RX_OVERFLOW
    printf("RX Overflow.\n");
    radio_strobe(RF_SIDLE);
    break;
  case 22: //TX_OVERFLOW
    printf("TX Overflow.\n");
    radio_strobe(RF_SIDLE);
    break;
    
  case 13: //RX Mode
    /* The screen will dim down in this mode unless we power cycle it,
       but that's not this module's responsibility.  See issue #56 on
       Github.
    */
    
    break;
  }

}

//! Keypress handler for the beacon applet.
int beacon_keypress(char ch){
  switch(ch){
  case '7':
    if(radio_getstate()==1){
      //Schedule packet.
      packet_tx((uint8_t*)
		//Actual message.
		CALLSIGN " BEACON BEACON BEACON\n"
		//Pad the extra with NULLs.
		"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
		"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
		"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"

		//But only send the proper length.
		,LEN);
    }
    break;
  case '/':
    /* Receiving packets isn't yet supported.*/
    if(radio_getstate()==1){
      packet_rxon();
    }
    
    break;
  case '0':
    packet_rxoff();
  }

  return 0;
}
