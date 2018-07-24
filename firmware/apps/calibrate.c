/*! \file calibrate.c
  \brief RTC calibration tool.
  
  The number selected on the screen is the RTC calibration.  Positive
  numbers add 2ppm apiece to the crystal duration, while negative
  numbers subtract 4ppm from the duration.

  1ppm roughly coresponds to 0.1728 seconds per day, so a value of +63
  makes the day 21.8 seconds longer and a value of -63 makes the day
  43.5 seconds shorter.  Your correct adjustment will certainly be
  smaller than these extremes, and I've found that -28 works very well
  for most GoodWatch21 boards.
*/

#include<msp430.h>
#include "api.h"
#include "applist.h"
#include "apps/calibrate.h"

//! Calibration offset, in units of 2ppm.
static int calibration = RTCCALIBRATE;

//! Enforce the calibration setting.
void calibrate_enforce(){
  int newrtcctl2=0;
  if(calibration<0){
    newrtcctl2|=0x80;
    newrtcctl2|=(0-calibration);
  }else{
    newrtcctl2=calibration;
  }
  RTCCTL2=newrtcctl2;
}

//! Draw the calibrate selection.
void calibrate_draw(){
  // Indicate that we're in the calibrate by setting the plus and
  // minus digits.
  lcd_number(calibration);
}

//! Change the selected applet.
int calibrate_keypress(char c){
  switch(c){
   case '+':
    calibration++;
    calibrate_enforce();
    break;
  case '-':
    calibration--;
    calibrate_enforce();
    break;
  }

  return 1;//Redraw after the press.
}

//! On entry, load calibration.
void calibrate_init(){
  calibration=(0x3F & RTCCTL2);

  //Invert the calibration if it's negative.
  if(RTCCTL2&0x80)
    calibration=0-calibration;
}


//! On exit, set the calibrate app.
int calibrate_exit(){
  // Return zero because we're done.
  return 0;
}
