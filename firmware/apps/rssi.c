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
     next app if we don't.
     
     While the radio is on, we keep it in the IDLE state except for
     once per second, when we briefly jump to receive mode to measure
     the RSSI.
   */
  if(has_radio){
    radio_on();
    radio_writesettings(0);
    radio_writepower(0x25);
    radio_setfreq(434000000);
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
  static int i=0;
  static int rssi=0x5;
  
  switch(ch){
  case '/':
    lcd_number(radio_getfreq()/10000);
    setperiod(2,1);
    
    break;
  default:
    //Update the signal strength only once a second.
    if((i++&0x04)==4){
      rssi=radio_getrssi();
    }
    lcd_number(rssi);
    lcd_string("RSSI");
    switch(rssi&0xF0){
    case 0xF0:
    case 0xE0:
    case 0xD0:
      setperiod(0,1);
    case 0xC0:
      setperiod(1,1);
    case 0xB0:
      setperiod(2,1);
    case 0xA0:
      setperiod(3,1);
    case 0x90:
      setperiod(4,1);
    case 0x80:
      setperiod(5,1);
    case 0x70:
      setperiod(6,1);
    case 0x60:
      setperiod(7,1);
    }
    break;
  }

}

