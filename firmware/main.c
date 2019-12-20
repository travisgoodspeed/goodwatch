/*! \file main.c

  \brief Main module.  This version initializes the LCD and then
   drops to a low power mode, letting the WDT update the display each
   quarter second, or in the handler of a keypress.
*/

#include <msp430.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "api.h"
#include "applist.h"
#include "dmesg.h"
#include "rng.h"

//! Power On Self Test
int post(){
  if(LCDBIV || (LCDBCTL1&LCDNOCAPIFG)){
    /* When the LCD cap is missing, of the wrong value, or
       mis-soldered, the charge pump will be disabled out of
       self-protection.  This looks like a normally dark screen
       abruptly fading to nothing, but you might still be able to
       see the response at the right angle.
    */
    lcd_string("lcd  lcd");
    printf("LCD Error.\n");
  }else if(UCSCTL7&2){
    /* This flag is triggered when the 32kHz crystal has a fault, such
       as if it is not populated or if a little drag of solder reaches
       ground.  Watches with this problem will lose minutes a day, but
       the '430 will fall back to an internal oscillator so that
       non-watch functions still work.
     */
    lcd_string(" crystal");
    printf("32kHz crystal error.");
  /*Can't run this test because of an unfixed errata.
  }else if(has_radio && RF1AIFERR & 1){
    lcd_string("RF  LOWV");
  */
  }else if(has_radio && RF1AIFERR & 2){
    lcd_string("RF OPERR");
  }else if(has_radio && RF1AIFERR & 4){
    lcd_string("RFOUTERR");
  }else if(has_radio && RF1AIFERR & 8){
    lcd_string("RF OVERW");
  }else{
    /* Return zero if everything is hunky dory.
     */
    lcd_string("all good");
    return 0;
  }

  printf("POST failure.\n");
  //We had a failure, indicated above.
  return 1;
}

//! Main method.
int main(void) {
  WDTCTL = WDTPW + WDTHOLD; // Stop WDT

  //Initialize the printf library.
  init_printf(NULL, dmesg_putc);

  //Initialize the various modules.
  dmesg_init();

  printf("RNG ");
  srand(true_rand()); // we do this as early as possible, because it messes with clocks

  printf("REF ");
  ref_init();

  printf("LCD ");
  lcd_init();
  
  lcd_zero();
  printf("rtc ");
  lcd_string("RTC INIT");
  rtc_init();

  lcd_zero();
  printf("osc ");
  lcd_string("OSC INIT");
  ucs_init();
  
  //Recognize the CPU model.
  descriptor_dump();
  

  lcd_zero();
  printf("buzz ");
  lcd_string("BUZZINIT");
  buzz_init();
  /* Startup tones kill the watch in low battery.
  tone(NOTE_C7, 500);
  tone(NOTE_E7, 500);
  tone(NOTE_G7, 500);
  tone(NOTE_A8, 500);
  tone(NOTE_C8, 500);
  */


  lcd_zero();
  printf("cp ");
  lcd_string("CP  INIT");
  codeplug_init();
  
  lcd_zero();
  printf("rad ");
  lcd_string("RAD INIT");
  radio_init();

  printf("Beginning POST.\n");
  lcd_string("POSTPOST");
  // Run the POST until it passes.
  while(post());

  //Finally we initialize the application.
  lcd_zero();
  lcd_string("APP INIT");
  app_init();
  
  //Keys and buttons are initialized *after* the application.
  printf("key ");
  key_init();
  printf("but ");
  sidebutton_init();
  
  //Unused IO pins must be outputs.
  PJDIR |=  0xF;
  PJOUT &= ~0xF;

  //UART must come after the sidebuttons.
  uart_init();
  
  // Setup and enable WDT 250ms, ACLK, interval timer
  WDTCTL = WDT_ADLY_250;
  SFRIE1 |= WDTIE;

  printf("Booted.\n");
  __bis_SR_register(LPM3_bits + GIE);        // Enter LPM3
  while(1){
    /* These dots oughtn't appear in dmesg, because the main thread
       ought to be paused with all processing in interrupts.  If this
       code executes, we've got a power management problem.
     */
    printf(".");
  }
}

//! Watchdog Timer interrupt service routine, calls back to handler functions.
void __attribute__ ((interrupt(WDT_VECTOR))) watchdog_timer (void) {
  static int latch=0;
  static int oldsec;

  /* When the UART is in use, we don't want to hog interrupt time, so
     we will silently return.
  */
  if(uartactive)
    return;

  if(sidebutton_mode()){
    /* So if the side button is being pressed, we increment the latch
       and move to the next application.  Some applications, such as
       the calculator, might hijack the call, so if we are latched for
       too many polling cycles, we forcibly revert to the clock
       application.
    */

    //Politely move to the next app if requested.
    if(!(latch++)){
      //lcd_zero();
      app_next();
    }
    
    //Force a shift to the home if held for 4 seconds (16 polls)
    if(latch>16)
      app_forcehome();
    
    /* Similarly, we'll reboot if the SET/PRGM button has been held
       for 10 seconds (40 polls).  We'll draw a countdown if getting
       close, so there's no ambiguity as to whether the chip reset.
       
       The other features of this button are handled within each
       application's draw function.
    */
    if(latch>40)
      PMMCTL0 = PMMPW | PMMSWPOR;
    
  }else{
    latch=0;
  }

  /* The applet is drawn four times per second, except for the clock,
     which is only drawn once a second.  We handle double-buffering,
     so that incomplete drawings won't be shown to the user, but
     everything else is the app's responsibility. */
  if(applet->draw!=clock_draw || (oldsec!=RTCSEC)){
    oldsec=RTCSEC;
    
    lcd_predraw();
    app_draw(0); //Unforced, because it's a regular timer.
    lcd_postdraw();
  }
}
