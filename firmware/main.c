/* Main module.  This version just initializes the LCD and then drops
   to a low power mode, letting the WDT do the work on a slow
   interval.
*/

#include <msp430.h>

#include "lcd.h"

int main(void) {
  WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
  
  lcd_init();
  rtc_init();

  // Setup and enable WDT 1000ms, ACLK, interval timer
  WDTCTL = WDT_ADLY_250;
  SFRIE1 |= WDTIE;
  
  __bis_SR_register(LPM0_bits + GIE);		// Enter LPM3 w/interrupt
  while(1);
}

// Watchdog Timer interrupt service routine, calls back to handler functions.
void __attribute__ ((interrupt(WDT_VECTOR))) watchdog_timer (void) {
  //lcd_wdt();
  //draw_time();

  //Blink the colon faster than the RTC.
  setcolon(3);
}
