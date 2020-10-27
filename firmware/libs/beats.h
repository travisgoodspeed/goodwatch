/*! \file beats.h
  \brief Swatch Internet Time (aka beats) library
  
  Convert standard time to swatch internet time, which is a metric time system that divides each
  mean solar day into 1000 "beats". This code counts the number of seconds since midnight UTC+1.
  It's a bit lossy because each "beat" is 86.4 seconds, but oh well

  https://en.wikipedia.org/wiki/Swatch_Internet_Time
*/

uint16_t clock2beats(uint16_t hours, uint16_t minutes, uint16_t seconds, int16_t utc_offset);