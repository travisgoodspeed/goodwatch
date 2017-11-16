/*! \file rtc.h
  \brief Real Time Clock library.
*/

//! Flash buffer containing the manufacturing time.
extern unsigned char *romsavetime;

//! Configure the Real Time Clock
void rtc_init();

//! Sets the DOW from the calendar date.
void rtc_setdow();
