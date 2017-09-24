/* RTC driver for the GoodWatch.
 */

#include <msp430.h>

#include "rtc.h"
#include "lcd.h"
#include "lcdtext.h"



static int h=17, m=9, s=0;

//! Get the hour.
int get_hour(){
  return h%24;
}
//! Get the minute.
int get_minute(){
  return m%60;
}
//! Get the second.
int get_second(){
  return s%60;
}

//! Increase the clock by one second.
void tick_clock(){
  if(++s>60){
    m++;
    s-=60;
  }
  if(m>60){
    h++;
    m-=60;
  }
  if(h>24){
    h-=24;
  }
}

void draw_time(){
  int hour=get_hour();
  int min=get_minute();
  int sec=get_second();
  
  lcd_zero();
  lcd_digit(7,hour/10);
  lcd_digit(6,hour%10);
  setcolon(1);
  lcd_digit(4,min/10);
  lcd_digit(3,min%10);
  //space
  lcd_digit(1,sec/10);
  lcd_digit(0,sec%10);
}

void rtc_init(){
  // Setup RTC Timer
  RTCCTL01 = RTCTEVIE + RTCSSEL_2 + RTCTEV_0; // Counter Mode, RTC1PS, 8-bit ovf
                                            // overflow interrupt enable
  RTCPS0CTL = RT0PSDIV_2;                   // ACLK, /8, start timer
  RTCPS1CTL = RT1SSEL_2 + RT1PSDIV_3;       // out from RT0PS, /16, start timer

  //__bis_SR_register(LPM3_bits + GIE);
}

void __attribute__ ((interrupt(RTC_VECTOR))) RTC_ISR (void){
  //switch(__even_in_range(RTCIV,16))
  switch(RTCIV&~1){
    case 0: break;                          // No interrupts
    case 2: break;                          // RTCRDYIFG
    case 4:                                 // RTCEVIFG
      tick_clock();
      draw_time();
      break;
    case 6: break;                          // RTCAIFG
    case 8: break;                          // RT0PSIFG
    case 10: break;                         // RT1PSIFG
    case 12: break;                         // Reserved
    case 14: break;                         // Reserved
    case 16: break;                         // Reserved
    default: break;
  }
  
}
