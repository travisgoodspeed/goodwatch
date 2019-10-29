/* \file settime.c
   \brief Applet for setting the time.  Not in the normal menu.
   
   This applet sets the time.  Previously, this was a part of clock.c,
   but because that module needs to be as lean and efficient as
   possible, it made sense to separate these features out into a
   separate application for setting them time.
*/

#include <msp430.h>
#include "api.h"
#include "clock.h"

//! If non-zero, we are setting the time.
static int settingclock=0;


//! Initialize the time setter.
void settime_init(){
  clock_init();
  settingclock=1;
}


//! Really quits back to the clock applet.
static void reallyexit(){
  //Return to the clock applet.
  app_reset();
  draw_time(1);
}

//! Move to the next digit, or finally exit the applet.
int settime_exit(){
  if(settingclock && settingclock<=13){
    //Setting the time, so jump to next digit.
    settingclock++;
    return 1;
  }else{
    //Not setting the time, so we move back to our own app by undoing
    //the app_set() call in clock.c.
    app_reset();
    draw_time(1);
    return 1;
  }
}

//! Draw the setting time.
void settime_draw(int forced){
  static int flicker=0;
  flicker^=1;

  /* The SET button will move us out of the programming mode. */
  if(sidebutton_set()){
    //Wait for the button to be released.
    while(sidebutton_set());
    //Let's quit!
    reallyexit();
    return;
  }
  
  //Zero the second hand if we're not yet to the date.  ("Hacking.")
  if(settingclock<5)
    SetRTCSEC(0);

  //First we draw the entire thing, then we blink the second being
  //set.
  if(settingclock<5)
    draw_time(1);
  else
    draw_date();

  switch(settingclock){
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
    
  case 5:        //Second
    /* We no longer set the seconds, but rather hold them at zero
       until the user moves back them into the date.  Mechanical watch
       experts call this 'hacking.'
    */ 
    settingclock++;
  case 6:
    settingclock++;
    
  case 7:        //Year
    if(flicker)
      lcd_cleardigit(7);
    break;
  case 8:
    if(flicker)
      lcd_cleardigit(6);
    break;
  case 9:
    if(flicker)
      lcd_cleardigit(5);
    break;
  case 10:
    if(flicker)
      lcd_cleardigit(4);
    break;
    
  case 11:        //Month
    if(flicker)
      lcd_cleardigit(3);
    break;
  case 12:
    if(flicker)
      lcd_cleardigit(2);
    break;
    
  case 13:       //Day
    if(flicker) 
      lcd_cleardigit(1);
    break;
  case 14:
    if(flicker)
      lcd_cleardigit(0);
    break;
  }
}

//! A button has been pressed for the clock.
int settime_keypress(char ch){
  unsigned char inputdigit=0;
  
  //We only handle numbers here.
  if((ch&0x30)==0x30)
    inputdigit=ch&0x0F;
  else
    return 1;
    
  switch(settingclock){
  case 1:         //Hour
    SetRTCHOUR(inputdigit*10+RTCHOUR%10);
    settingclock++;
    break;
  case 2:
    SetRTCHOUR(RTCHOUR-RTCHOUR%10+inputdigit);
    settingclock++;
    break;
  case 3:         //Minute
    SetRTCMIN(inputdigit*10+RTCMIN%10);
    settingclock++;
    break;
  case 4:
    SetRTCMIN(RTCMIN-RTCMIN%10+inputdigit);
    settingclock++;
    break;
    
    /* We no longer set the seconds, but rather hold them at zero
       until the user moves back them into the date.  Mechanical watch
       experts call this 'hacking.'
    */
  case 7:        //Year
    SetRTCYEAR(inputdigit*1000+RTCYEAR%1000);
    settingclock=8;
    break;
  case 8:
    SetRTCYEAR(RTCYEAR-RTCYEAR%1000+inputdigit*100+RTCYEAR%100);
    settingclock++;
    break;
  case 9:
    SetRTCYEAR(RTCYEAR-RTCYEAR%100+inputdigit*10+RTCYEAR%10);
    settingclock++;
    break;
  case 10:
    SetRTCYEAR(RTCYEAR-RTCYEAR%10+inputdigit);
    settingclock++;
    break;
    
  case 11:        //Month
    SetRTCMON(inputdigit*10+RTCMON%10);
    settingclock++;
    break;
  case 12:
    SetRTCMON(RTCMON-RTCMON%10+inputdigit);
    settingclock++;
    break;
    
  case 13:       //Day
    SetRTCDAY(inputdigit*10+RTCDAY%10);
    settingclock++;
    break;
  case 14:
    SetRTCDAY(RTCDAY-RTCDAY%10+inputdigit);
    settingclock++;
    
  default:
    /* Once we've exceeded the count, it's time to return to the
       normal mode.
    */
    settingclock=0;
    reallyexit();
  }

  //Update the DOW.  We could save some cycles by only doing this if
  //the date changes, but we don't bother.
  rtc_setdow();

  //Do redraw.
  return 1;
}

