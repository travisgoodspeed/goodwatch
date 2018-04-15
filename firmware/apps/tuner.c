 /*! \file tuner.c
  \brief Tuning application and RSSI tool.
  
  This quick little tool allows the user to tune the radio between
  configured and VFO frequencies.
  
  + and - change the channel, / shows the current working frequency,
  and 7 measures the signal strength on the current channel.
  
  The = button can be used to set a VFO frequency.
*/

#include<stdio.h>
#include<stdlib.h>
#include<msp430.h>
#include "api.h"

//Draws the codeplug name.
static void draw(){
  lcd_string(codeplug_name());
}

//! Enter the tuner tool.
void tuner_init(){
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
    codeplug_setfreq();
    radio_strobe(RF_SCAL);
  }else{
    app_next();
  }
}
//! Exit the radio tool.
int tuner_exit(){
  /* Always turn the radio off at exit.
   */
  radio_off();

  //Allow the exit.
  return 0;
}

//! Last character pressed in the tuner.
static int vfosetmode=0;
static int rssi=0x5;

//! Tuner keypress callback.
int tuner_keypress(char ch){
  switch(ch){
  default:
    draw();
    break;
  case '+':  //Next channel.
    codeplug_next();
    codeplug_setfreq();
    break;
  case '-':  //Previous channel.
    codeplug_prev();
    codeplug_setfreq();
    break;
  case '/':  //Show the frequency.
    lcd_number(codeplug_getfreq()/10);
    //setperiod(5,1);  //Periods are controlled by RSSI right now.
    //setperiod(2,1);
    break;
  case '=':  //Set a frequency.  Not yet working.
    vfosetmode=!vfosetmode;
    break;
  case '7':  //Show snapshot of scalar RSSI.
    lcd_number(rssi);
    lcd_string("RSSI");
    break;
  }

  return 1;//Redraw.
}

void clearperiods(){
  int i;
  for(i=0; i<8; i++)
    setperiod(i,0);
}

//! Draw the screen and increase the count.
void tuner_draw(){
  static int i=0;
  
  /* Every other frame, we grab the signal strength.  Highest
     stays. */
  if(!(i&1))
    rssi=radio_getrssi();
  
  //Draw the new strength.
  clearperiods();
  switch(rssi&0xF0){
  case 0xF0:
  case 0xE0:
    setperiod(0,1);
  case 0xD0:
    setperiod(1,1);
  case 0xC0:
  case 0xB0:
    setperiod(2,1);
  case 0xA0:
  case 0x90:
    setperiod(3,1);
  case 0x80:
  case 0x70:
    setperiod(4,1);
  case 0x60:
  case 0x50:
    setperiod(5,1);
  case 0x40:
  case 0x30:
    setperiod(6,1);
  case 0x20:
    setperiod(7,1);
  }
  
}


