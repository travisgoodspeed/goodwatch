/*! \file rtcasm.h
  \brief Hand assembly to set the RTC.
  
  The CC430 has an awful errata that makes setting the time a bit of a
  gamble, based on CPU alignment.  These functions ought to work
  around it, if we've linked rtcasm.S properly and not one NOP has
  been changed.

*/

#ifndef RTC_H_
#define RTC_H_

//! Set the year.
extern int SetRTCYEAR(int year);
//! Set the month.
extern int SetRTCMON(int month);
//! Set the day.
extern int SetRTCDAY(int day);
//! Set the day of the week.
extern int SetRTCDOW(int dow);
//! Set the hour.
extern int SetRTCHOUR(int hour);
//! Set the minute.
extern int SetRTCMIN(int min);
//! Set the second.
extern int SetRTCSEC(int sec);

//! Get TIM0.
extern int GetRTCTIM0(void);
//! Get TIM1
extern int GetRTCTIM1(void);
//! Get the date.
extern int GetRTCDATE(void);
//! Get the year.
extern int GetRTCYEAR(void);

#endif /*RTC_H_*/
