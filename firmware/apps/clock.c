/* This is the main clock application, which is seen by default
   whenever the watch has nothing else to do.  In addition to telling
   the time, holding a button allows you to see additional information,
   such as the date or the voltage.
*/

#include <msp430.h>

#include "api.h"

//! Draws the time.
void draw_time(){
  static int i=0;
  unsigned int hour=RTCHOUR;
  unsigned int min=RTCMIN;
  unsigned int sec=RTCSEC;
  
  lcd_digit(7,hour/10);
  lcd_digit(6,hour%10);
  lcd_cleardigit(5); //Space
  setcolon((++i>>1)&1);
  lcd_digit(4,min/10);
  lcd_digit(3,min%10);
  lcd_cleardigit(2); //Space
  lcd_digit(1,sec/10);
  lcd_digit(0,sec%10);

  setam(hour<12);
  setpm(hour>=12);
}

//! Draws the date as yyyy.mm.dd
void draw_date(){
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


//! Entry to the clock app.
void clock_init(){
  lcd_zero();
}
//! Exit when the button is pressed.
int clock_exit(){
  return 0;
}

//! Draws the clock face in the main application.
void clock_draw(){
  static char oldch=0;
  char ch=getchar();

  if(oldch!=ch)
    lcd_zero();
  
  switch(ch){
  case '/':
    draw_date();
    break;
  case '7':
    lcd_hex(0xdeadbeef);
    break;


    /* For now, we set the time by the 1,2,3 buttons and date by 4,5,6
       buttons. */
  case '1':
    if(oldch!=ch)
      RTCHOUR=(RTCHOUR+1)%24;
    draw_time();
    break;
  case '2':
    if(oldch!=ch)
      RTCMIN=(RTCMIN+1)%60;
    draw_time();
    break;
  case '3':
    if(oldch!=ch)
      RTCSEC=0;
    draw_time();
    break;
  case '4':
    if(RTCYEAR<2016)
      RTCYEAR=2016;
    if(oldch!=ch)
      RTCYEAR=(RTCYEAR+1)%2020;
    draw_date();
    break;
  case '5':
    if(oldch!=ch)
      RTCMON=(RTCMON+1)%13;
    draw_date();
    break;
  case '6':
    if(oldch!=ch)
      RTCDAY=(RTCDAY+1)%32;
    draw_date();
    break;


    
  case 0:
    draw_time();
    break;
  default:
    lcd_hex(ch);
  }

  oldch=ch;
}

