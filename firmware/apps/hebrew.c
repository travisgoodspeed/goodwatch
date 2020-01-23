/*! \file hebrew.c
  \brief Hebrew Calendar Application
  
  This applet serves to display the time, but it also displays the
  Hebrew day, month, year, and dow.  This will be the first applet to
  use fallthrough mode to also be accessible from the main screen.
  
  Later, this might be merged with the Shabbat app, but until it
  stabilizes, they will be separate.
  
  This applet only works in GCC8.

*/

#include <stdint.h>

#include "api.h"
#include "applist.h"

#include "libs/hebrew.h"

static struct hebrew_date hdate;

//! Updates the hebrew date from the secular.
static void hebrew_update(){
  static uint16_t oldhash=0;
  uint16_t newhash=RTCYEAR+RTCMON+RTCDAY;  //Could be a better hash.  Must be fast!

  //Don't recalculate if the date is already right!
  if(newhash==oldhash)
    return;

  //Don't run if we fail the selftest.
  if(!hebrew_selftest()){
    printf("Selftest failed.\n");
    return;
  }

  oldhash=newhash;
  
  //Convert the date by running through days since 1900-1-1.
  uint32_t udate=hebrew_get_universal(RTCYEAR, RTCMON, RTCDAY);
  //uint32_t udate=hebrew_get_universal(1900, 1, 1);
  hebrew_calendar_from_universal(udate, &hdate);
}

//! Draw the day of the week.
static void hebrew_draw_dow(){
  lcd_string(hdaysofweek[RTCDOW]);
}

//! Draw the day and month.
static void hebrew_draw_date(){
  //Call this before anything that uses the date!
  hebrew_update();

  /* Because the month index is ambiguous between the Biblical and
     Civil calendars, we must display the month.  hmonths[] is an
     array of abreviated strings that are right-aligned on the
     display, leaving the two leftmost digits for the number.
     
     Also, Adar 1 is the "extra month" in leap years, and we must
     manually override the table as an exception to that.
   */
  if(hdate.month==12 && hebrew_calendar_leap_year_p(hdate.year))
    lcd_string("  adar1");
  else
    lcd_string(hmonths[hdate.month]);
  lcd_digit(7, (hdate.day/10)%10);
  lcd_digit(6, hdate.day%10);
}

//! Draw the year, month, and day.
static void hebrew_draw_year(){
  //Call this before anything that uses the date!
  hebrew_update();
  
  unsigned int year=hdate.year;
  unsigned int month=hdate.month;
  unsigned int day=hdate.day;

  lcd_digit(7,(year/1000)%10);
  lcd_digit(6,(year/100)%10);
  lcd_digit(5,(year/10)%10);
  lcd_digit(4,year%10);
  setperiod(4,1);
  setcolon(0);
  lcd_digit(3,month/10);
  lcd_digit(2,month%10);
  setperiod(2,1);
  lcd_digit(1,day/10);
  lcd_digit(0,day%10);

  setam(0);
  setpm(0);
}



//! Enter the Hebrew application.
void hebrew_init(){
  
}

//! Exit the Hebrew application.
int hebrew_exit(){
  return 0;
}

static char lastchar=0;

//! Draw the Hebrew screen.
void hebrew_draw(){
  /* When complete, we'll just exit right out of the Hebrew applet and
     let fallthrough mode do its job.  For now, just render the time
     innefficiently and hope for the best.
  */
  
  //Draw the time, usually.
  if(!lastchar)
    draw_time(1);
}

//! Keypress handler for the hebrew applet.
int hebrew_keypress(char ch){
  lcd_zero();

  /* We use the top row as it would be used in the Clock applet, and
     also the second row from the bottom, which will be dedicated to
     fallthrough mode.
   */
  switch(lastchar=ch){
  case '8':
  case '2':
    hebrew_draw_year();
    break;
    
  case '9':
  case '3':
    hebrew_draw_dow();
    break;
    
  case '/':
  case '-':
    hebrew_draw_date();
    break;


  default:
    return 1;
  }

  return 1;
}


