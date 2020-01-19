/*! \file hebrew.h
  \brief Hebrew calendar library.
*/


struct hebrew_date { int year; int month; int day; };

extern uint32_t hebrew_get_universal(int y, int m, int d);
extern void hebrew_calendar_from_universal (uint32_t udate, struct hebrew_date *result);

extern const char *hdaysofweek[];
extern const char *hmonths[];
