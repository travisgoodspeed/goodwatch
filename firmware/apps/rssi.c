/*! \file rssi.c
  \brief Handy RSSI Tool.
  
  This is a quick little tool for measuring the RSSI on the current
  frequency.
  
*/

#include<stdio.h>
#include<stdlib.h>
#include<msp430.h>
#include "api.h"


//! Enter the RSSI tool.
void rssi_init(){
  /* Begin by initializing the radio if we have one, or jumping to the
     next app if we don't.  I don't quite know why, but this app won't
     work until after we cycle through the Morse app.  Maybe a known
     erratum?
   */
  if(has_radio){
    radio_on();
    radio_writesettings(0);
    radio_writepower(0x25);
    radio_setfreq(434000000);
    __delay_cycles(400);
    radio_setfreq(434000000);

    //Strobe to receive mode, so we can catch the RSSI.o
    printf("Got 0x%02x strobing SRX in rssi_init().\n",
	   radio_strobe(RF_SRX));
  }else{
    app_next();
  }
}
//! Exit the radio tool.
int rssi_exit(){
  /* Always turn the radio off at exit.
   */
  radio_off();

  //Allow the exit.
  return 0;
}
//! Draw the screen and increase the count.
void rssi_draw(){
  char ch=getchar();
  static int i=0, rssi=0x5;
  
  switch(ch){
  case '/':
    lcd_number(radio_getfreq()/10.0);
    setperiod(5,1);
    setperiod(2,1);
    
    break;
  default:  //Show that we're using rssi.
    //if((i++&0x04)==4){
    //radio_setfreq(434000000);
      rssi=radio_getrssi();
    //}
    lcd_number(rssi);
    lcd_string("RSSI");
    break;
  }

}

