/* This is the main clock application, which is seen by default
   whenever the watch has nothing else to do.  In addition to telling
   the time, holding a button allows you to see additional information,
   such as the date or the voltage.
*/

#include <msp430.h>

#include "api.h"

//! Entry to the clock app.
void clock_init(){
  lcd_zero();
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
  case '+':
    app_next();
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
