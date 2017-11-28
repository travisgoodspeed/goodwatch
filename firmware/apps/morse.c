/*! \file morse.c
  \brief Handy morse code tool for 70cm.
  
  This is a handy little application for playing with the radio.  It
  will be modified to use radio interrupt callbacks in the future, and
  any particularly large or complicated radio tools should go in a
  different applet.
  
*/


#include "api.h"


//! Enter the radio tool.
void morse_init(){
  /* Power management being king, we shouldn't initialize the radio
     until we need it, but for now we'll start it at entry to keep
     things simple.
   */
  radio_init();
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
    radio_morse("--... ...-- ");
    break;
  case '1':
    lcd_string("      CQ");
    radio_morse("-.-. --.- ");
    break;
  case '0':
    lcd_string("       K");
    radio_morse("-.- ");
    break;
    
    
  case '/':
    /* TODO Replace this with a configurable callsign. */
    radio_morse("-.- -.- ....- ...- --..    ");
    lcd_string("  KK4VCZ");
    break;
    
  default:  //Show the callsign by default.
    lcd_string("   MORSE");
    break;
  }

}

