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
#include<string.h>
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

static void clearperiods(){
  int i;
  for(i=0; i<8; i++)
    setperiod(i,0);
}

int vfosetmode_bufferi;
char vfosetmode_buffer[9];

static void vfosetmode_apply(){
  long f=atol(vfosetmode_buffer);

  vfosetmode=0;
  printf("Setting frequency to %ld\n",f);
  codeplug_setvfofreq((float) f);
}

static void vfosetmode_keypress(char ch){
  //Numbers populate a buffer character.
  if(ch>='0' && ch<='9')
    vfosetmode_buffer[vfosetmode_bufferi++]=ch;

  //Exit when we have all characters or = is pressed again.
  if(ch=='=' || vfosetmode_bufferi==8){
    vfosetmode_apply();
  }
}

static void vfosetmode_draw(){
  /* This is called four times a second when we are setting the VFO
     frequency.  Activate the mode by pressing the = button.
   */
  clearperiods();
  lcd_string(vfosetmode_buffer);
  setperiod(5,1);
  setperiod(2,1);
}


//! Tuner keypress callback.
int tuner_keypress(char ch){
  if(vfosetmode){
    vfosetmode_keypress(ch);
    return 0;
  }
  
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
    break;
  case '=':  //Set a VFO frequency.
    vfosetmode=1;
    //Clear all eight digits, plus null terminator.
    memset(vfosetmode_buffer,'0',9);
    vfosetmode_bufferi=0;
    break;
  case '7':  //Show snapshot of scalar RSSI.
    lcd_number(rssi);
    lcd_string("RSSI");
    break;
  }

  return 1;//Redraw.
}


//! Draw the screen and increase the count.
void tuner_draw(){
  static int i=0;
  
  //No sense using the radio when we don't yet know the frequency.
  if(vfosetmode){
    vfosetmode_draw();
    return;
  }
  
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


