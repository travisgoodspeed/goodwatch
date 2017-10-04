/* RTC driver for the GoodWatch.
 */

#include <msp430.h>
#include <string.h>

#include "rtc.h"
#include "lcd.h"
#include "lcdtext.h"

static unsigned long magicword __attribute__ ((section (".noinit")));
static unsigned char ramsavetime[8] __attribute__ ((section (".noinit")));
static unsigned char *romsavetime=(unsigned char*) 0xFF00;

//! Save the times to RAM.  Must be fast.
static void rtc_savetime(){
  ramsavetime[0]=RTCHOUR;
  ramsavetime[1]=RTCMIN;
  ramsavetime[2]=RTCSEC;
  *((int*) &(ramsavetime[4]))=RTCYEAR;
  ramsavetime[6]=RTCMON;
  ramsavetime[7]=RTCDAY;
  
  //Set the magic word, so we'll know the time is good.
  magicword=0xdeadbeef;
}

//! Load the time from RAM or ROM
static void rtc_loadtime(){
  //Use the RAM copy if it is reasonable.
  if(magicword!=0xdeadbeef){
    memcpy(ramsavetime,romsavetime,8);
  }
  
  //Default clock comes from flash.
  //RTC will fix any corruption.
  RTCHOUR = ramsavetime[0]   %24;
  RTCMIN =  ramsavetime[1]   %60;
  RTCSEC =  ramsavetime[2]   %60;
  RTCYEAR = (ramsavetime[4]+(ramsavetime[5]<<8)) % 4096;
  RTCMON = ramsavetime[6];
  RTCDAY = ramsavetime[7];  //Not sure why, but we have to set
  RTCDAY = ramsavetime[7];  //the day twice.  RTC Errata?
}


//! Initializes the clock with the timestamp from memory.
void rtc_init(){
  // Setup RTC Timer

  // Calendar Mode, RTC1PS, 8-bit ovf
  // overflow interrupt enable
  RTCCTL01 = RTCTEVIE + RTCSSEL_2 + RTCTEV_0 + RTCMODE;
  RTCPS0CTL = RT0PSDIV_2;                   // ACLK, /8, start timer
  RTCPS1CTL = RT1SSEL_2 + RT1PSDIV_3;       // out from RT0PS, /16, start timer

  rtc_loadtime();
}

void __attribute__ ((interrupt(RTC_VECTOR))) RTC_ISR (void){
  //Save the time once a minute, so that when we reboot, we loose just
  //a few seconds.  We might later decide to call this in the
  //rendering loop.
  rtc_savetime();
  
  //We don't really handly these, but might want to in the future.
  switch(RTCIV&~1){
    case 0: break;                          // No interrupts
    case 2: break;                          // RTCRDYIFG
    case 4: break;                          // RTCTEVIFG
    case 6: break;                          // RTCAIFG Alarm
    case 8: break;                          // RT0PSIFG
    case 10: break;                         // RT1PSIFG
    case 12: break;                         // Reserved
    case 14: break;                         // Reserved
    case 16: break;                         // Reserved
    default: break;
  }
}
