/*! \file hebrew.h
  \brief Hebrew calendar library.
  
  We try to define the bare minimum of exported symbols.  See main()
  in libs/hebrew.c for the host-side test cases and apps/hebrew.c for
  the device-side GUI code.
*/


struct hebrew_date { uint16_t year; uint16_t month; uint16_t day; };

extern uint32_t hebrew_get_universal(uint16_t y, uint16_t m, uint16_t d);
extern void hebrew_calendar_from_universal (uint32_t udate, struct hebrew_date *result);
extern int hebrew_calendar_leap_year_p (uint16_t year);

extern int hebrew_selftest();


extern const char *hdaysofweek[];
extern const char *hmonths[];
