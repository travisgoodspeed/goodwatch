/*! \file calibrate.h
  \brief RTC calibration tool.
*/


#include "config.h"


//! Enforce the calibration setting.
void calibrate_enforce();

//! Draw the calibrate selection.
void calibrate_draw(int forced);

//! Change the calibration.
int calibrate_keypress(char c);

//! On entry, load calibration.
void calibrate_init();
//! On exit, set the calibrate app.
int calibrate_exit();

#ifndef RTCCALIBRATE
#define RTCCALIBRATE 0
#endif

