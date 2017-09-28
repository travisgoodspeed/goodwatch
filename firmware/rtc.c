/* RTC driver for the GoodWatch.
 */

#include <msp430.h>

#include "rtc.h"
#include "lcd.h"
#include "lcdtext.h"




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
  RTCHOUR = *((unsigned char*) 0xFF00)   %24;
  RTCMIN =  *((unsigned char*) 0xFF01)   %60;
  RTCSEC =  *((unsigned char*) 0xFF02)   %60;
  RTCYEAR = *((unsigned int*) 0xFF04) % 4096;
  RTCMON = *((unsigned char*) 0xFF06);
  RTCDAY = *((unsigned char*) 0xFF07);  //Not sure why, but we have to set
  RTCDAY = *((unsigned char*) 0xFF07);  //the day twice.  RTC Errata?
}

void __attribute__ ((interrupt(RTC_VECTOR))) RTC_ISR (void){
  //Display the hex value if we ever drop here.
  //lcd_hex(0xC1000000|RTCIV);
  
  //We don't really handly these, but might want to in the future.
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
