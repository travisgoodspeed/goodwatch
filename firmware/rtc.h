/*! \file rtc.h
  \brief Real Time Clock library.
*/

//! Flash buffer containing the manufacturing time.
extern unsigned char *romsavetime;

//! Configure the Real Time Clock
void rtc_init();
