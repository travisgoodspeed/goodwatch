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
    lcd_string("  MORSE ");
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

//! A button has been pressed for Morse.
int morse_keypress(char ch){
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

  case '=':
    /* Pressing the equals button switches to raw Morse mode, where the
       mode button causes a carrier to be transmitted.  Exit by pressing
       the button again.
       
       Set-button is a straight key.
    */
    lcd_string(" RAW CW ");
    lcd_postdraw();
    __delay_cycles(6000);
    do{
      if(sidebutton_mode() || getchar()=='+'){
	if(radio_getstate()==1){
	  radio_strobe(RF_STX);
	  __delay_cycles(300);
	}
      }else{
	if(radio_getstate()!=1){
	  radio_strobe(RF_SIDLE);
	  __delay_cycles(300);
	}
      }
    }while(getchar()!='=');//!sidebutton_set());
    lcd_string("RAW EXIT");
    lcd_postdraw();
    __delay_cycles(6000);
    break;
    
  case '/':
    lcd_string("        ");
    lcd_string(CALLSIGN);
    lcd_postdraw();
    radio_morse(CALLSIGN);
    break;
  }
  lcd_string("  MORSE ");
  return 0;//Redraw after keypress.
}

//! Draw the screen.
void morse_draw(){
  //We don't actually draw here.
}

