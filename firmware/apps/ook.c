/*! \file ook.c
  \brief Example OOK Application

  
  Howdy y'all,
  
  This is a quick little comm demo that sends OOK commands to INSMA
  Modules.  Additional remotes can be added without forking this
  module by placing their data rates (MDMCFG4 and MDMCFG3) and packets
  into the OOKBUTTONS array of your config.h.
  
  You can buy this particular transmitter from Amazon as ``INSMA
  433Mhz Wireless RF Switch 328ft Long Range DC 12V 4CH Channel
  Wireless Remote Control Switch, DC12V Relay Receiver Module,
  Transmitter Toggle Switch RF Relay (2 Transmitter & 1 Receiver).''
  
  These packets are based on my own device, but you can use the
  learning button to train your radio to it.  Bits and timings were
  initially reversed with Universal Radio Hacker, then a single packet
  was prototyped in Python through the Monitor feature.  Once this
  packet was properly compared to the original, a steady stream of
  packets was created by the ook_packettx() callback.
  
  The 'bits' are 341 µs long for a symbol rate of 2.93207 kilobaud,
  but because the protocol is designed in terms of short and long
  pulses, an E nybble represents a long pulse and an 8 represents a
  short pulse.

  These are the four packets from my remote:
  
  A 0000e8e8ee88e88ee888eee8888e8000
  B 0000e8e8ee88e88ee888eee888e88000
  C 0000e8e8ee88e88ee888eee88e888000
  D 0000e8e8ee88e88ee888eee8e8888000

  
  There is no preamble; rather, packets begin on a rising edge in the
  third byte.  The receiver expects many packets in a row, so we start
  a new transmittion in the callback after each transmission and we
  raise the main clock frequency to be able to react in time.  For
  other receivers, you might be able to get away with a single
  transmission.
  
  Further details and a tutorial for adding your own remote are in the
  wiki.
  
  https://github.com/travisgoodspeed/goodwatch/wiki/OOK_Example
  
  --Travis
  
*/

#include<stdio.h>
#include<string.h>
#include<msp430.h>
#include "api.h"

//Fixed packet length is easier on our packet library.
#define LEN 16

/* Settings were prototyped first in Python.  This is basic OOK with
   no preamble, no CRC, and 341µs symbol times.
 */
static const uint8_t ook_settings[]={
  //Change these to change the rate.
  //MDMCFG4, OOKMDMCFG4,      // Modem Configuration
  //MDMCFG3, OOKMDMCFG3,      // Modem Configuration
  //These rest are consistent for all OOK emulation.
  MDMCFG2, 0x30,      // Modem Configuration, no sync
  FREND0 , 0x11,      // Front End TX Configuration
  FSCAL3 , 0xE9,      // Frequency Synthesizer Calibration
  FSCAL2 , 0x2A,      // Frequency Synthesizer Calibration
  FSCAL1 , 0x00,      // Frequency Synthesizer Calibration
  FSCAL0 , 0x1F,      // Frequency Synthesizer Calibration
  PKTCTRL0, 0x00,     // Packet automation control, fixed length without CRC.
  PKTLEN,  LEN,       // PKTLEN    Packet length.
  0,0
};

//! Array of keys for button pressing.
static const char * const button_array[] = {
  /* These were recorded with Universal Radio Hacker (URH).  You might
     need to adjust both these packets and the symbol periods defined
     above. */
  OOKBUTTONS
};


/*
//! Local function that returns an OOK array index from a button.  -1 on failure.
static int button2index(char c){
  //For now, only numbers.  Might change this later to have even rows of four buttons.
  if(c<='9' && c>='0'){
    return c-'0';
  }

  //Otherwise failure.
  return -1;
}
*/

//! Handle an incoming packet.
void ook_packetrx(uint8_t *packet, int len){
  printf("Not yet supported.\n");
}

static char lastch=0;


//! Send a packet.
static void transmit(int index){
  //Packet begins on the third byte.
  packet_tx((uint8_t*) button_array[index]+2,
	    LEN);
}
//! Set the rate.
static void setrate(int index){
  //First two bytes are the rate.
  radio_writereg(MDMCFG4, ((uint8_t*) button_array[index])[0]);
  radio_writereg(MDMCFG3, ((uint8_t*) button_array[index])[1]);
}

//! Called after a transmission, or on a button press.
void ook_packettx(){
  /* Schedule next packet if a number is being held.  We already set
     the bitrate in the keypress handler.
  */
  if(lastch<='9' && lastch>='0'){
    transmit(lastch-'0');
  }
}


//! Enter the OOK transmitter application.
void ook_init(){
  /* Skip this application if we haven't got a radio.
   */
  if(!has_radio){
    app_next();
  }

  lcd_string("     OOK");

  printf("%d button entries are available.\n", sizeof(button_array)/2);
}

//! Exit the OOK application.
int ook_exit(){
  //Cut the radio off.
  radio_off();
  //Allow the exit.
  return 0;
}

//! Draw the OOK screen.
void ook_draw(){
  int state=radio_getstate();

  switch(state){
  case 0:
  case 1:
    //lcd_string("     OOK");
    break;
  case 19: //RX IDLE between transmit packets.
    lcd_string("TRANSMIT");
    break;
  case 22: //TX_OVERFLOW
    printf("TX Overflow.\n");
    radio_strobe(RF_SIDLE);
    break;
  default:
    lcd_hex(state);
    break;
  }
}

//! Keypress handler for the ook applet.
int ook_keypress(char ch){
  /* When a key is first pressed, we call ook_packettx() to kick off a
     transmission.  That function is also the callback from the packet
     library, so it will keep running in a loop until the key is
     released.
   */
  if( (lastch=ch) && ch>='0' && ch<='9' ){
    //Radio settings.
    radio_on();
    radio_writesettings(ook_settings);
    setrate(ch-'0'); //First two bytes are the packet rate.
    radio_writepower(0x25);
    //Set a frequency manually rather than using the codeplug.
    radio_setfreq(433960000);

    //This handler will be called back as the packet finished transmission.
    ook_packettx();
  }else{
    //Shut down the radio when the button is released.
    radio_off();
    lcd_zero(); //Clear the clock and radio indicators.
    lcd_string("     OOK");
  }
  return 0;
}
