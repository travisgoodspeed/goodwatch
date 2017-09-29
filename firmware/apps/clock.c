/* This is the main clock application, which is seen by default
   whenever the watch has nothing else to do.  In addition to telling
   the time, holding a button allows you to see additional information,
   such as the date or the voltage.
*/

#include <msp430.h>

#include "api.h"

/* When this is non-zero, we are in programming mode.  The number
   indicates the field being set.
 */
static int settingclock=0;


//! Draws the time.
static void draw_time(){
  unsigned int hour=RTCHOUR;
  unsigned int min=RTCMIN;
  unsigned int sec=RTCSEC;
  
  lcd_digit(7,hour/10);
  lcd_digit(6,hour%10);
  lcd_cleardigit(5); //Space
  setcolon(1);
  lcd_digit(4,min/10);
  lcd_digit(3,min%10);
  lcd_cleardigit(2); //Space
  lcd_digit(1,sec/10);
  lcd_digit(0,sec%10);

  setam(hour<12);
  setpm(hour>=12);
}

//! Draws the date as yyyy.mm.dd
static void draw_date(){
  unsigned int year=RTCYEAR;
  unsigned int month=RTCMON;
  unsigned int day=RTCDAY;

  lcd_digit(7,(year/1000)%10);
  lcd_digit(6,(year/100)%10);
  lcd_digit(5,(year/10)%10);
  lcd_digit(4,year%10);
  setcolon(0);
  lcd_digit(3,month/10);
  lcd_digit(2,month%10);
  lcd_digit(1,day/10);
  lcd_digit(0,day%10);

  setam(0);
  setpm(0);
}

int flicker=0;

//! Draws whatever is being set
static void draw_settingtime(char ch){
  
  int inputdigit=0;

  flicker^=1;

  //We only handle numbers here.
  if((ch&0x30)==0x30)
    inputdigit=ch&0x0F;
  else
    ch=0;

  //First we draw the entire thing, then we blink the second being
  //set.
  if(settingclock<7)
    draw_time();
  else
    draw_date();

  switch(settingclock){
  case 1:         //Hour
    if(flicker)
      lcd_cleardigit(7);
    if(ch){
      RTCHOUR=inputdigit*10+RTCHOUR%10;
      settingclock++;
    }
    break;
  case 2:
    if(flicker)
      lcd_cleardigit(6);
    if(ch){
      RTCHOUR=RTCHOUR-RTCHOUR%10+inputdigit;
      settingclock++;
    }
    break;
  case 3:         //Minute
    if(flicker)
      lcd_cleardigit(4);
    if(ch){
      RTCMIN=inputdigit*10+RTCMIN%10;
      settingclock++;
    }
    break;
  case 4:
    if(flicker)
      lcd_cleardigit(3);
    if(ch){
      RTCMIN=RTCMIN-RTCMIN%10+inputdigit;
      settingclock++;
    }
    break;
    
  case 5:        //Second
    if(flicker)
      lcd_cleardigit(1);
    if(ch){
      RTCSEC=inputdigit*10;
      settingclock++;
    }
    break;
  case 6:        //Second
    if(flicker)
      lcd_cleardigit(0);
    if(ch){
      RTCSEC=RTCSEC-RTCSEC%10+inputdigit;
      settingclock++;
    }
    break;
  case 7:        //Year
    if(flicker)
      lcd_cleardigit(7);
    if(ch){
      RTCYEAR=inputdigit*1000+RTCYEAR%1000;
      settingclock++;
    }
    break;
  case 8:
    if(flicker)
      lcd_cleardigit(6);
    if(ch){
      RTCYEAR=RTCYEAR-RTCYEAR%100+inputdigit*100+RTCYEAR%10;
      settingclock++;
    }
    break;
  case 9:
    if(flicker)
      lcd_cleardigit(5);
    if(ch){
      RTCYEAR=RTCYEAR-RTCYEAR%100+inputdigit*10+RTCYEAR%10;
      settingclock++;
    }
    break;
  case 10:
    if(flicker)
      lcd_cleardigit(4);
    if(ch){
      RTCYEAR=RTCYEAR-RTCYEAR%10+inputdigit;
      settingclock++;
    }
    break;
  case 11:        //Month
    if(flicker)
      lcd_cleardigit(3);
    if(ch){
      RTCMON=inputdigit*10+RTCMON%10;
      settingclock++;
    }
    break;
  case 12:
    if(flicker)
      lcd_cleardigit(2);
    if(ch){
      RTCMON=RTCMON-RTCMON%10+inputdigit;
      settingclock++;
    }
    break;
    
  case 13:       //Day
    if(flicker) 
      lcd_cleardigit(1);
    if(ch){
      RTCDAY=inputdigit*10+RTCDAY%10;
      settingclock++;
    }
    break;
  case 14: //Day
    if(flicker)
      lcd_cleardigit(0);
    if(ch){
      RTCDAY=RTCDAY-RTCDAY%10+inputdigit;
      settingclock++;
    }
    break;
  default:
    /* Once we've exceeded the count, it's time to return to the
       normal mode.
     */
    settingclock=0;
  }
}


//! Entry to the clock app.
void clock_init(){
  lcd_zero();
}
//! Exit clock when the sidebutton is pressed, unless we are programming.
int clock_exit(){
  if(settingclock){
    //Setting the time, so jump to next digit.
    settingclock++;
    return 1;
  }else{
    //Not setting the time, so just move on to next app.
    return 0;
  }
}

//! Draws the clock face in the main application.
void clock_draw(){
  static char oldch=0;
  char ch=getchar();


  if(oldch!=ch)
    lcd_zero();

  /* The SET button will move us into the programming mode. */
  if(sidebutton_set()){
    settingclock=!settingclock;
  }

  if(settingclock)
    draw_settingtime(ch);
  else
    switch(ch){
    case '/':
      //Hold / to draw the date.
      draw_date();
      break;
    case '7':
      lcd_hex(0xdeadbeef);
      break;
    case 0:
      // Draw the time by default.
      draw_time();
      break;
    default:
      lcd_hex(ch);
    }

  oldch=ch;
}

