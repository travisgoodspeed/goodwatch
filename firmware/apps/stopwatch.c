/* This is a simple stop watch, which begins counting as the + key is
   hit and clears the count when the 0 key is pressed.  Because the RTC is busy
   with the real time, we could in quarter seconds as the display is updated.

   We do not count time or store the count when in other applicaitons.

*/

#include "api.h"
#include "lcdtext.h"

static int counting=0;
static int count;

//! Entry to the stopwatch app.
void stopwatch_init(){
  //Zero the count.
  count=0;
  //Start off without counting.
  counting=1;
}

//! Exit form the stopwatch app.
int stopwatch_exit(){
  //Give up without a fight when the mode button is pressed.
  return 0;
}

//! Draws the time.
void stopwatch_draw_time(){
  static int i=0;
  unsigned int hour, min, sec, subsec;

  //Increment the count if we're counting.
  if(counting)
    count++;
  //Divide the count.
  hour=((count>>2)/60/60);
  min=((count>>2)/60)%60;
  sec=(count>>2)%60;
  subsec=(count&3)*25;

  if(hour){
    lcd_digit(7,hour/10);
    lcd_digit(6,hour%10);
    lcd_cleardigit(5); //Space
    setcolon(i++&1);
    lcd_digit(4,min/10);
    lcd_digit(3,min%10);
    lcd_cleardigit(2); //Space
    lcd_digit(1,sec/10);
    lcd_digit(0,sec%10);
  }else{
    lcd_digit(7,min/10);
    lcd_digit(6,min%10);
    lcd_cleardigit(5); //Space
    setcolon(i++&1);
    lcd_digit(4,sec/10);
    lcd_digit(3,sec%10);
    lcd_cleardigit(2); //Space
    lcd_digit(1,subsec/10);
    lcd_digit(0,subsec%10);
  }

  
  //AM/PM off.
  setam(0);
  setpm(0);
}


//! Draw the stopwatch app and handle its input.
void stopwatch_draw(){
  stopwatch_draw_time();
}
