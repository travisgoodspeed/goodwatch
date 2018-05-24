/*! \file calibrate.h
  \brief RTC calibration tool.
*/


#include "config.h"


//! Enforce the calibration setting.
void calibrate_enforce();

//! Draw the calibrate selection.
void calibrate_draw();

//! Change the selected applet.
int calibrate_keypress(char c);

//! Draw the calibrate selected.
void calibrate_drawselected();

//! On entry, load calibration.
void calibrate_init();
//! On exit, set the calibrate app.
int calibrate_exit();

#ifndef RTCCALIBRATE
#define RTCCALIBRATE 0
#endif

