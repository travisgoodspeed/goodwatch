/*! \file stopwatch.c
  
  \brief Stopwatch application.
   
  This is a simple stop watch, which begins counting as the + key is
  hit and clears the count when the 0 key is pressed.  Because the
  RTC is busy with the real time, we could in quarter seconds as the
  display is updated.

  We do not count time or store the count when in other applications.
*/

#include "api.h"


static int counting=0;

/* It's a crying shame, but you'll break the code if you increase this
   count to a long, because repeated divisions in rendering will take
   longer than the ~8k cycles we have per interrupt when running at
   32kHz and the stopwatch will run too slow.

   To work around this, we maintain the count of hours separately from
   the general second count.
 */
static unsigned int count, hour;


//! Entry to the stopwatch app.
void stopwatch_init(){
  //Zero the count.
  count=0;
  hour=0;
  //Start off without counting.
  counting=0;
}

//! Exit form the stopwatch app.
int stopwatch_exit(){
  //Give up without a fight when the mode button is pressed.
  return 0;
}

//! Draws the time.
void stopwatch_draw_time(){
  static char oldch=0;
  char ch;
  unsigned int min, sec, subsec;

  /* The stopwatch is special in that it never times out.  Be very
     careful when doing this, because a minor bug might kill the
     battery.
   */
  app_cleartimer();
  
  /* For now, we support only two buttons.  + stops and starts the
     count, while 0 resets the counter.
   */
  ch=getchar();
  if(oldch==0)
    switch(ch){
    case '+':
      counting=!counting;
      break;
    case '0':
      //Clear both the second counter and the hour counter.
      count=0;
      hour=0;
      break;
    }
  oldch=ch;

  //Increment the count if we're counting.
  if(counting)
    count++;
  //Divide the count.
  min=((count>>2)/60)%60;
  sec=(count>>2)%60;
  subsec=(count&3)*25;

  //Handle hour rollover.
  if(((count>>2)/60)>60){
    count=0;
    hour++;
  }
  
  if(hour){
    lcd_digit(7,hour/10);
    lcd_digit(6,hour%10);
    lcd_cleardigit(5); //Space
    setcolon((count>>1)&1);
    lcd_digit(4,min/10);
    lcd_digit(3,min%10);
    lcd_cleardigit(2); //Space
    lcd_digit(1,sec/10);
    lcd_digit(0,sec%10);
  }else{
    lcd_digit(7,min/10);
    lcd_digit(6,min%10);
    lcd_cleardigit(5); //Space
    setcolon((count>>1)&1);
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
