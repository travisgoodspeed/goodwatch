/* Main module.  This version just initializes the LCD and then drops
   to a low power mode, letting the WDT do the work on a slow
   interval.
*/

#include <msp430.h>
#include <string.h>

#include "lcd.h"
#include "rtc.h"
#include "keypad.h"

//Initialize the XT1 crystal, and stabilize it.
void xtal_init(){
  P5SEL |= BIT0 + BIT1;                     // Select XT1
  UCSCTL6 |= XCAP_3;                        // Internal load cap

  // Loop until XT1,XT2 & DCO stabilizes
  do{
    UCSCTL7 &= ~(XT1LFOFFG + DCOFFG);
                                            // Clear LFXT1,DCO fault flags
    SFRIFG1 &= ~OFIFG;                      // Clear fault flags
  }while (SFRIFG1 & OFIFG);                 // Test oscillator fault flag

  UCSCTL6 &= ~(XT1DRIVE_3);                 // Xtal is now stable, reduce drive
                                            // strength
  //See page 125 of the family guide.
  //UCSCTL4 = SELM_3 + SELS_0 + SELA_0;  //XT1 for ACLK and SMCLK, MCLK from DCO.
  UCSCTL4 = SELM_0 + SELS_0 + SELA_0;  //XT1 for everything; very slow CPU.
}

int main(void) {
  WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
  
  lcd_init();
  xtal_init();
  rtc_init();
  key_init();

  // Setup and enable WDT 250ms, ACLK, interval timer
  WDTCTL = WDT_ADLY_250;
  SFRIE1 |= WDTIE;

  // Turn off SVSH, SVSM
  PMMCTL0_H = 0xA5;
  SVSMHCTL = 0;
  SVSMLCTL = 0;
  PMMCTL0_H = 0x00;

  __bis_SR_register(LPM3_bits +GIE);         // Enter LPM3
  //__bis_SR_register(LPM0_bits + GIE);	     // Enter LPM0 w/interrupt
  while(1);
}

// Watchdog Timer interrupt service routine, calls back to handler functions.
void __attribute__ ((interrupt(WDT_VECTOR))) watchdog_timer (void) {
  lcd_wdt();
}
