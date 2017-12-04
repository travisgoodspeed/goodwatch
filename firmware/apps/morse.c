/*! \file morse.c
  \brief Handy morse code tool for 70cm.
  
  This is a handy little application for playing with the radio.  It
  will be modified to use radio interrupt callbacks in the future, and
  any particularly large or complicated radio tools should go in a
  different applet.
  
  7 transmits "73", 1 transmits "CQ CQ CQ CQ", 0 transmits "K", and /
  transmits the owner's callsign.
  
*/

#include<msp430.h>
#include "api.h"


//! Enter the Morse application.
void morse_init(){
  /* Power management being king, we shouldn't initialize the radio
     until we need it, but for now we'll start it at entry to keep
     things simple.
   */
  if(has_radio){
    radio_on();
    radio_writesettings(0);
    radio_writepower(0x25);
    codeplug_setfreq();
    radio_strobe(RF_SCAL);
  }else{
    app_next();
  }
}
//! Exit the Morse app.
int morse_exit(){
  /* Always turn the radio off at exit.
   */
  radio_off();

  //Allow the exit.
  return 0;
}

//! Draw the screen.
void morse_draw(){
  char ch=getchar();
  
  switch(ch){
  case '7':
    lcd_string("      73");
    lcd_postdraw();
    radio_morse("73");
    break;
  case '1':
    lcd_string("      CQ");
    lcd_postdraw();
    radio_morse("CQ CQ CQ");
    break;
  case '0':
    lcd_string("       K");
    lcd_postdraw();
    radio_morse("K");
    break;
    
    
  case '/':
    lcd_string("        ");
    lcd_string(CALLSIGN);
    lcd_postdraw();
    radio_morse(CALLSIGN);
    break;
    
  default:  //Show that we're using morse.
    lcd_string("  NNORSE");
    break;
  }

}

