/*! \file ook.c
  \brief Example OOK Application

  
  Howdy y'all,
  
  This is a quick little comm demo that sends OOK commands to INSMA
  Modules.  Fork this one if you need to emulate other OOK devices, by
  adjusting button_array[] and the modem bitrate.
  
  You can buy this particular transmitter from Amazon as ``INSMA
  433Mhz Wireless RF Switch 328ft Long Range DC 12V 4CH Channel
  Wireless Remote Control Switch, DC12V Relay Receiver Module,
  Transmitter Toggle Switch RF Relay (2 Transmitter & 1 Receiver).''
  
  These particular packets are based on my own device, but you can use
  the learning button to train your radio to it.  Bits and timings
  were initially reversed with Universal Radio Hacker, then a single
  packet was prototyped in Python through the Monitor feature.  Once
  this packet was properly compared to the original, a steady stream
  of packets was created by the ook_packettx() callback.
  
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
  wiki.  https://github.com/travisgoodspeed/goodwatch/wiki/OOK_Example
  
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
  MDMCFG4, 0x86,      // Modem Configuration
  MDMCFG3, 0xD9,      // Modem Configuration
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

  "\x00\x00\xe8\xe8\xee\x88\xe8\x8e\xe8\x88\xee\xe8\x88\x8e\x80\x00", //A
  "\x00\x00\xe8\xe8\xee\x88\xe8\x8e\xe8\x88\xee\xe8\x88\xe8\x80\x00", //B
  "\x00\x00\xe8\xe8\xee\x88\xe8\x8e\xe8\x88\xee\xe8\x8e\x88\x80\x00", //C
  "\x00\x00\xe8\xe8\xee\x88\xe8\x8e\xe8\x88\xee\xe8\xe8\x88\x80\x00"  //D
};


//! Handle an incoming packet.
void ook_packetrx(uint8_t *packet, int len){
  printf("Not yet supported.\n");
}

static char lastch=0;

//! Called after a transmission, or on a button press.
void ook_packettx(){
  /* Schedule next packet if the right key is being held.
     
     Buttons for A,B,C,D are 0,1,2,3 or 0,1,4,7.
  */
  switch(lastch){
  case '0':
    packet_tx((uint8_t*) button_array[0],
	      LEN);
    break;
  case '1':
    packet_tx((uint8_t*) button_array[1],
	      LEN);
    break;
  case '2': case '4':
    packet_tx((uint8_t*) button_array[2],
	      LEN);
    break;
  case '3': case '7':
    packet_tx((uint8_t*) button_array[3],
	      LEN);
    break;
  }
  }


//! Enter the OOK transmitter application.
void ook_init(){
  /* This enters the application.
     We ignore the codeplug frequency and set our own.
   */
  if(has_radio){
    //Faster processing time, for rapid packet succession.
    ucs_fast();
    
    radio_on();
    radio_writesettings(ook_settings);
    radio_writepower(0x25);
  
    //Set a frequency manually rather than using the codeplug.
    radio_setfreq(433960000);
  }else{
    app_next();
  }
}

//! Exit the OOK application.
int ook_exit(){
  //Cut the radio off.
  radio_off();
  ucs_slow();
  //Allow the exit.
  return 0;
}

//! Draw the OOK screen.
void ook_draw(){
  int state=radio_getstate();

  if(state==1){
    lcd_string("     OOK");
  }else{
    lcd_string("TRANSMIT");
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

//! Keypress handler for the ook applet.
int ook_keypress(char ch){
  /* When a key is first pressed, we call ook_packettx() to kick off a
     transmission.  That function is also the callback from the packet
     library, so it will keep running in a loop until the key is
     released.
   */
  if( (lastch=ch) )
    ook_packettx();
  return 0;
}
