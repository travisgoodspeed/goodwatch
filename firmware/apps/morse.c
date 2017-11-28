/*! \file morse.c
  \brief Handy morse code tool for 70cm.
  
  This is a handy little application for playing with the radio.  It
  will be modified to use radio interrupt callbacks in the future, and
  any particularly large or complicated radio tools should go in a
  different applet.
  
*/

#include<msp430.h>
#include "api.h"


//! Enter the radio tool.
void morse_init(){
  /* Power management being king, we shouldn't initialize the radio
     until we need it, but for now we'll start it at entry to keep
     things simple.
   */
  if(has_radio){
    radio_on();
    radio_writesettings(0);
    radio_writepower(0x25);
    radio_setfreq(434000000);
    radio_strobe(RF_SCAL);
  }else{
    app_next();
  }
}
//! Exit the radio tool.
int morse_exit(){
  /* Always turn the radio off at exit.
   */
  radio_off();

  //Allow the exit.
  return 0;
}
//! Draw the screen and increase the count.
void morse_draw(){
  char ch=getchar();
  
  switch(ch){
  case '7':
    lcd_string("      73");
    lcd_postdraw();
    radio_morse("--... ...-- ");
    break;
  case '1':
    lcd_string("      CQ");
    lcd_postdraw();
    radio_morse("-.-. --.- ");
    break;
  case '0':
    lcd_string("       K");
    lcd_postdraw();
    radio_morse("-.- ");
    break;
    
    
  case '/':
    /* TODO Replace this with a configurable callsign. */
    lcd_string("  KK4VCZ");
    lcd_postdraw();
    radio_morse("-.- -.- ....- ...- --..    ");
    break;
    
  default:  //Show that we're using morse.
    lcd_string("  NNORSE");
    break;
  }

}

