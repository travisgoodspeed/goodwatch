/*! \file hebrew.h
  \brief Hebrew calendar library.
*/


struct hebrew_date { int year; int month; int day; };

extern void hebrew_calendar_from_universal (int udate, struct hebrew_date *result);
