/* RTC driver for the GoodWatch.
 */

#include <msp430.h>

#include "rtc.h"
#include "lcd.h"
#include "lcdtext.h"



//! Get the hour.
int get_hour(){
  return RTCHOUR;
}
//! Get the minute.
int get_minute(){
  return RTCMIN;
}
//! Get the second.
int get_second(){
  return RTCSEC;
}

void draw_time(){
  static int i=0;
  int hour=get_hour();
  int min=get_minute();
  int sec=get_second();
  
  lcd_zero();
  lcd_digit(7,hour/10);
  lcd_digit(6,hour%10);
  setcolon(i++&1);
  lcd_digit(4,min/10);
  lcd_digit(3,min%10);
  //space
  lcd_digit(1,sec/10);
  lcd_digit(0,sec%10);
}

//! Initializes the clock with the timestamp from memory.
void rtc_init(){
  // Setup RTC Timer

  // Counter Mode, RTC1PS, 8-bit ovf
  // overflow interrupt enable
  RTCCTL01 = RTCTEVIE + RTCSSEL_2 + RTCTEV_0 + RTCMODE;
  
  RTCPS0CTL = RT0PSDIV_2;                   // ACLK, /8, start timer
  RTCPS1CTL = RT1SSEL_2 + RT1PSDIV_3;       // out from RT0PS, /16, start timer

  //Default clock comes from flash.
  //RTC will fix any corruption.
  RTCHOUR = *((unsigned char*) 0xFF00)   %12;
  RTCMIN =  *((unsigned char*) 0xFF01)   %60;
  RTCSEC =  *((unsigned char*) 0xFF02)   %60;
 
  //__bis_SR_register(LPM3_bits + GIE);
}

void __attribute__ ((interrupt(RTC_VECTOR))) RTC_ISR (void){
  //Display the hex value if we ever drop here.
  lcd_hex(0xC1000000|RTCIV);
  
  //switch(__even_in_range(RTCIV,16))
  switch(RTCIV&~1){
    case 0: break;                          // No interrupts
    case 2: break;                          // RTCRDYIFG
    case 4:                                 // RTCEVIFG
      //Might do something here in counter mode.
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
