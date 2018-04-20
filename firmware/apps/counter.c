/*! \file counter.c
  \brief Frequency Counter Application

  Howdy y'all,
  
  Once upon a time, a neighbor was in Heidelberg for the Troopers
  conference, and he wanted to join the staff radio networks.  So he
  performed the out-of-band-TX mod to his Kenwood TH-D74, and borrowed
  a laptop and SDR to find the staff simplex frequency.  From an audio
  recording, he identified the squelch tones and was on the air.
  
  But why did he need an SDR, when his wristwatch ought to have been
  able to quickly find the peak during transmission?  This applet
  seeks to remedy that problem by providing a basic frequency counter,
  displaying the center frequency of the strongest signal yet
  observed.  It is loosely based on Michael Ossmann's spectrum
  analyzer, written for the Girltech IMME in 2010.

  This implementation does a single sweep from 410 to 470 MHz when the
  0 button is pressed.  A better implementation would perform a second
  pass with a narrower filter to better identify the center frequency
  after a sweep.
  
  --Travis
  
*/

#include<stdio.h>
#include<string.h>
#include<msp430.h>
#include "api.h"


/* Settings were prototyped first in Python.  This is basic OOK with
   no preamble, no CRC, and 341µs symbol times.
 */
#define LEN 16
static const uint8_t counter_settings[]={
  /* IF of 457.031 kHz */
  FSCTRL1, 0x12,
  FSCTRL0, 0x00,
  
  /* disable 3 highest DVGA settings */
  //AGCCTRL2 |= AGCCTRL2_MAX_DVGA_GAIN;
  
  /* frequency synthesizer calibration */
  FSCAL3, 0xEA,
  FSCAL2, 0x2A,
  FSCAL1, 0x00,
  FSCAL0, 0x1F,
  
  /* "various test settings" */
  TEST2, 0x88,
  TEST1, 0x31,
  TEST0, 0x09,
  
  /* no automatic frequency calibration */
  MCSM0, 0,


  /* Filter bandwidth */
  //MDMCFG4, 0xEC, /* 67.708333 kHz */
  //MDMCFG4, 0x0C, /* 812.5 kHz */
  MDMCFG4, 0x6C, /* 270.833333 kHz */

  //End with null terminator.
  0,0
};


/* This enum manages the state machine for the frequency counter.  The
   state will be IDLE before and after the sweep.
 */
static enum {IDLE, SWEEP} counter_state;
static int best_rssi;
static float best_freq;
#define MAX_FREQ  470000000  //Range is 410 to 470 for now.
#define MIN_FREQ  410000000  //Will open more bands later.
#define STEP_FREQ 000100000  //600 steps at 100kHz/Step
static float current_freq;

//! Try a given frequency, and update display if it's best.
static void try_freq(float freq){
  int rssi;
  //Set the frequency.
  radio_setfreq(freq);
  //Get the RSSI.
  rssi=radio_getrssi();

  //Compare it.
  if(rssi>best_rssi){
    best_freq=freq;
    best_rssi=rssi;
    lcd_hex(rssi);
  }
}

//! Try the next frequency in the set.
static void try_next(){
  //Enforce the range here.
  if(current_freq<MIN_FREQ)
    current_freq=MIN_FREQ;

  //Try the next center freq and step ahead.
  try_freq(current_freq);
  current_freq+=STEP_FREQ;

  //We're done!
  if(current_freq>MAX_FREQ){
    current_freq=MIN_FREQ;
    counter_state=IDLE;
  }
}

//! Enter the Counter application.
void counter_init(){
  /* This enters the application.
     We ignore the codeplug frequency and set our own.
   */
  if(has_radio){
    //Faster processing time, for rapid packet succession.
    ucs_fast();
    
    radio_on();
    radio_writesettings(counter_settings);

    //Initialize state variables.
    counter_state=IDLE;
    best_rssi=0;
    best_freq=0;
  }else{
    app_next();
  }
}

//! Exit the Counter application.
int counter_exit(){
  //Cut the radio off and drop the CPU frequency.
  radio_off();
  ucs_slow();
  
  //Allow the exit.
  return 0;
}

//! Draw the Counter screen.
void counter_draw(){
  switch(counter_state){
  case IDLE:
    if(best_rssi==0)
      lcd_string("CNT IDLE");
    else
      lcd_number(best_freq/10);
    break;
  case SWEEP:
    lcd_string("SWEEPING");
    
    break;
  }
}

//! Keypress handler for the Counter applet.
int counter_keypress(char ch){
  switch(ch){
  case '0': //Begin a new sweep.
    best_rssi=0;
    best_freq=0;
    counter_state=SWEEP;

    //Do the sweep.
    while(counter_state==SWEEP)
      try_next();
    
    break;
  }
  return 0;
}
