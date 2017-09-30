/* Main module.  This version just initializes the LCD and then drops
   to a low power mode, letting the WDT do the work on a slow
   interval.
*/

#include <msp430.h>
#include <string.h>

#include "lcd.h"
#include "lcdtext.h"
#include "rtc.h"
#include "keypad.h"
#include "apps.h"
#include "sidebutton.h"


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

//! Power On Self Test
int post(){
  if(LCDBIV || (LCDBCTL1&LCDNOCAPIFG)){
    /* When the LCD cap is missing, of the wrong value, or
       mis-soldered, the charge pump will be disabled out of
       self-protection.  This looks like a normally dark screen
       abruptly fading to nothing, but you might still be able to
       see the response at the right angle.
    */
    lcd_hex(0x1cd1cd);
  }else{
    /* Return zero if everything is hunky dory.
     */
    lcd_hex(0x600d600d);
    return 0;
  }
  return 1;
}

int main(void) {
  WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT

  //Initialize the various modules.
  lcd_init();
  xtal_init();
  rtc_init();
  key_init();
  sidebutton_init();
  app_init();

  // Setup and enable WDT 250ms, ACLK, interval timer
  WDTCTL = WDT_ADLY_250;
  SFRIE1 |= WDTIE;

  // Turn off SVSH, SVSM
  PMMCTL0_H = 0xA5;
  SVSMHCTL = 0;
  SVSMLCTL = 0;
  PMMCTL0_H = 0x00;

  // Run the POST until it passes.
  while(post());

  __bis_SR_register(LPM3_bits +GIE);         // Enter LPM3
  //__bis_SR_register(LPM0_bits + GIE);	     // Enter LPM0 w/interrupt
  while(1);
}

// Watchdog Timer interrupt service routine, calls back to handler functions.
void __attribute__ ((interrupt(WDT_VECTOR))) watchdog_timer (void) {
  static int latch=0;

  /* So if the side button is being pressed, we increment the latch
     and move to the next application.  Some applications, such as the
     calculator, might hijack the call, so if we are latched for too
     many poling cycles, we forcibly revert to the clock applicaiton.
   */
  if(sidebutton_mode()){
    lcd_zero();
    
    //Politely move to the next app if requested.
    if(!(latch++))
      app_next();
    
    //Force a shift to the home if held for 4 seconds (16 polls)
    if(latch>16)
      app_forcehome();
    
  }else{
    latch=0;
  }


  
  lcd_predraw();

  /* The applet is drawn four times per second.  We handle
     double-buffering, so that incomplete drawings won't be shown to
     the user, but everything else is the app's responsibility. */
  app_draw();

  lcd_postdraw();
}
