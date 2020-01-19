/*! \file hebrew.h
  \brief Hebrew calendar library.
*/


struct hebrew_date { uint16_t year; uint16_t month; uint16_t day; };

extern uint32_t hebrew_get_universal(uint16_t y, uint16_t m, uint16_t d);
extern void hebrew_calendar_from_universal (uint32_t udate, struct hebrew_date *result);

extern int hebrew_selftest();


extern const char *hdaysofweek[];
extern const char *hmonths[];
