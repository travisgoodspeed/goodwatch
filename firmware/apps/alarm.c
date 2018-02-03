/*! \file alarm.c
  \brief Alarm clock application.
  
*/

#include <msp430.h>
#include "api.h"


//! If non-zero, we are setting the alarm.
static int settingalarm=0;


//! Gets alarm set status
static int alarm_enabled() {
  return (RTCAHOUR & (1<<RTCAE) && RTCAMIN & (1<<RTCAE));
}


static void toggle_alarm(int enable) {
  if (enable){
    RTCAHOUR |= RTCAE;
    RTCAMIN |= RTCAE;
  } else {
    RTCAHOUR &= ~RTCAE;
    RTCAMIN &= ~RTCAE;
  }
}

//! Draws the alarm.
static void draw_alarm(){
  unsigned int hour=RTCAHOUR;
  unsigned int min=RTCAMIN;
  
  lcd_digit(7,hour/10);
  lcd_digit(6,hour%10);
  lcd_cleardigit(5); //Space
  setcolon(1);
  lcd_digit(4,min/10);
  lcd_digit(3,min%10);
  lcd_cleardigit(2); //Space
  lcd_char(1, 'a'); //Space
  lcd_char(0, 'l'); //Space
  
  if(alarm_enabled())
    setplus(1);
  else
    setplus(0);

  setam(hour<12);
  setpm(hour>=12);
}


//! Draws whatever is being set
static void draw_settingalarm(){
  static int flicker=0;
  
  flicker^=1;

  switch(settingalarm){
  case 1:         //Hour
    if(flicker)
      lcd_cleardigit(7);
    break;
  case 2:
    if(flicker)
      lcd_cleardigit(6);
    break;
  case 3:         //Minute
    if(flicker)
      lcd_cleardigit(4);
    break;
  case 4:
    if(flicker)
      lcd_cleardigit(3);
    break;
  }
}


//! Entry to the alarm app.
void alarm_init(){
  lcd_zero();
}
//! Exit alarm when the sidebutton is pressed, unless we are programming.
int alarm_exit(){
  if(settingalarm){
    //Setting the alarm, so jump to next digit.
    settingalarm++;
    return 1;
  }else{
    //Not setting the alarm, so just move on to next app.
    setplus(0);
    return 0;
  }
}

static char lastchar=0;

//! Draws the alarm time in the main application.
void alarm_draw(){
  static char ch=0;

  if(ch!=lastchar)
    lcd_zero();
  
  ch=lastchar;

  /* The SET button will move us into the programming mode. */
  if(sidebutton_set()){
    settingalarm=!settingalarm;
  }

  if(settingalarm)
    draw_settingalarm();
  else
    switch(ch){
     case '4':
      if (alarm_enabled())
        alarm_toggle(0);
      else
        alarm_toggle(1);
      break;
  }
}

//! A button has been pressed for the alarm.
void alarm_keypress(char ch){
  unsigned char inputdigit=0;
  lastchar=ch;
  
  if(settingalarm){
    //We only handle numbers here.
    if((ch&0x30)==0x30)
      inputdigit=ch&0x0F;
    else
      return;
    
    switch(settingalarm){
    case 1:         //Hour
      RTCAHOUR = inputdigit*10+RTCAHOUR%10;
      settingalarm++;
      break;
    case 2:
      RTCAHOUR = RTCAHOUR-RTCAHOUR%10+inputdigit;
      settingalarm++;
      break;
    case 3:         //Minute
      RTCAMIN = inputdigit*10+RTCAMIN%10;
      settingalarm++;
      break;
    case 4:
      RTCAMIN = RTCAMIN-RTCAMIN%10+inputdigit;
      settingalarm++;
      break;
    default:
      /* Once we've exceeded the count, it's time to return to the
     normal mode.
      */
      settingalarm=0;
    }

    //Update the DOW.  We could save some cycles by only doing this if
    //the date changes, but we don't bother.
    rtc_setdow();
  }
}
