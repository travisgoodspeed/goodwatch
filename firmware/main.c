/*! \file main.c

  \brief Main module.  This version just initializes the LCD and then
   drops to a low power mode, letting the WDT do the work on a slow
   interval.
*/

#include <msp430.h>
#include <string.h>
#include <stdio.h>

#include "api.h"
#include "dmesg.h"


//! Power On Self Test
/*
 * Run the tests for the POST
 * @return result of each test
 */
post_status post() {
  post_status status = {0, 0, 0, 0, 0, 0};
  if(LCDBIV || (LCDBCTL1&LCDNOCAPIFG)){
    /* When the LCD cap is missing, of the wrong value, or
       mis-soldered, the charge pump will be disabled out of
       self-protection.  This looks like a normally dark screen
       abruptly fading to nothing, but you might still be able to
       see the response at the right angle.
    */
    status.failed = 1;
    status.lcd_bad = 1;
  }
  if(UCSCTL7 & 2){
    /* This flag is triggered when the 32kHz crystal has a fault, such
       as if it is not populated or if a little drag of solder reaches
       ground.  Watches with this problem will lose minutes a day, but
       the '430 will fall back to an internal oscillator so that
       non-watch functions still work.
     */
    status.failed = 1;
    status.crystal_bad = 1;

    UCSCTL7 &= ~(XT1LFOFFG + DCOFFG);    // Clear LFXT1,DCO fault flags
    SFRIFG1 &= ~OFIFG;                   // Clear fault flags
  }
/*Can't run this test because of an unfixed errata, RF1A6.
  if(has_radio && RF1AIFERR & 1) {
    status.failed = 1;
    status.rf_low_vcore = 1;
  }
    */

  if(has_radio && RF1AIFERR & 2) {
    status.failed = 1;
    status.rf_operand_err = 1;
  }

  if(has_radio && RF1AIFERR & 4) {
    status.failed = 1;
    status.rf_out_data_err = 1;
  }

  if(has_radio && RF1AIFERR & 8) {
    status.failed = 1;
    status.rf_operand_overrite_err = 1;
  }

  // Clear radio fault flags
  if(has_radio) {
    RF1AIFERR = 0;
  }

  return status;
}

//! Display a post_status to the LCD
void display_post(post_status status) {
  if (status.lcd_bad) {
    lcd_string("lcd  lcd");
    printf("POST fail: LCD \n");
    __delay_cycles(16000);
  }
  if (status.crystal_bad) {
    lcd_string(" crystal");
    printf("POST fail: 32kHz crystal\n");
    __delay_cycles(16000);
  }
  if (status.rf_low_vcore) {
    lcd_string("RF  LOWV");
    printf("POST fail: Low RF core voltage");
    __delay_cycles(16000);
  }
  if (status.rf_operand_err) {
    lcd_string("RF OPERR");
    printf("POST fail: RF operand error\n");
    __delay_cycles(16000);
  }
  if (status.rf_out_data_err) {
    lcd_string("RFOUTERR");
    printf("POST fail: RF data out error\n");
    __delay_cycles(16000);
  }
  if (status.rf_operand_overrite_err) {
    lcd_string("RF OVERW");
    printf("POST fail: RF operand overrite error\n");
    __delay_cycles(16000);
  }

  if (status.failed) {
    lcd_string("err err");
  } else {
    lcd_string("all good");
  }
  __delay_cycles(16000);
}

//! Main method.
int main(void) {
  WDTCTL = WDTPW + WDTHOLD; // Stop WDT

  //Initialize the various modules.
  dmesg_init();
  printf("Initializing LCD ");
  lcd_init();
  
  lcd_zero();
  printf("rtc ");
  lcd_string("RTC INIT");
  rtc_init();

  lcd_zero();
  printf("key ");
  lcd_string("KEY INIT");
  key_init();
  
  lcd_zero();
  printf("but ");
  lcd_string("BUT INIT");
  sidebutton_init();
  
  lcd_zero();
  printf("osc ");
  lcd_string("OSC INIT");
  ucs_init();

  lcd_zero();
  lcd_string("UARTINIT");
  uart_init();

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
  post_status status;
  do {
    status = post();
    display_post(status);
  } while(status.failed);

  lcd_zero();
  lcd_string("APP INIT");
  app_init();
  

  // Setup and enable WDT 250ms, ACLK, interval timer
  WDTCTL = WDT_ADLY_250;
  SFRIE1 |= WDTIE;

  printf("Booted.\n");
  __bis_SR_register(LPM3_bits + GIE);        // Enter LPM3
  //__bis_SR_register(LPM2_bits + GIE);        // Enter LPM2
  //__bis_SR_register(LPM0_bits + GIE);	     // Enter LPM0 w/interrupt
  while(1){
    //printf("main while().\n");
    //uart_tx('T');
  }
}

//! Watchdog Timer interrupt service routine, calls back to handler functions.
void __attribute__ ((interrupt(WDT_VECTOR))) watchdog_timer (void) {
  static int latch=0;

  /* When the UART is in use, we don't want to hog interrupt time, so
     we will silently return.
  */
  if(uartactive)
    return;

  if(sidebutton_mode()){
    /* So if the side button is being pressed, we increment the latch
       and move to the next application.  Some applications, such as the
       calculator, might hijack the call, so if we are latched for too
       many poling cycles, we forcibly revert to the clock applicaiton.
    */
    
    lcd_zero();
    
    //Politely move to the next app if requested.
    if(!(latch++))
      app_next();
    
    //Force a shift to the home if held for 4 seconds (16 polls)
    if(latch>16)
      app_forcehome();

  
  }else if(sidebutton_set()){
    /* Similarly, we'll reboot if the SET/PRGM button has been held for 10
       seconds (40 polls).  We'll draw a countdown if getting close, so
       there's no ambiguity as to whether the chip reset.
       
       The other features of this button are handled within each application's
       draw function.
    */
    if(latch++>40)
      PMMCTL0 = PMMPW | PMMSWPOR;
   
  }else{
    latch=0;
  }

  /* The applet is drawn four times per second.  We handle
     double-buffering, so that incomplete drawings won't be shown to
     the user, but everything else is the app's responsibility. */
  lcd_predraw();
  app_draw();
  lcd_postdraw();
}
