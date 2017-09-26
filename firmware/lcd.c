/* Minimal LCD example, forked from a reference by G. Larmore at TI.
   The LCD pinout doesn't match ours yet, and will need to be
   modified to fit.  */

#include <msp430.h>
#include <string.h>

#include "lcdtext.h"
#include "rtc.h"
#include "apps.h"

volatile unsigned char *lcdm=&LCDM1;
volatile unsigned char *lcdbm=&LCDBM1;

//! Clears the LCD memory and blink memory.
void lcd_zero(){
  int i=0;
  
  for(i=0; i<=13; i++){
    lcdm[i]=0;
  }
}



//! Initialize the LCD memory and populate it with sample text.
void lcd_init() {
  int i;
  
  // Select LCD COM pins
  P5SEL |= (/*BIT5 |*/ BIT6 | BIT7);
  P5DIR |= (/*BIT5 |*/ BIT6 | BIT7);

  // Configure LCD_B
  // LCD_FREQ = ACLK/32/4, LCD Mux 4, turn on LCD
  // Charge pump generated internally at 3.44V, external bias (V2-V4) generation
  // Internal reference for charge pump
  LCDBCTL0 =  (LCDDIV0 + LCDDIV1 + LCDDIV2 + LCDDIV3 + LCDDIV4)| LCDPRE0 | LCD4MUX | LCDON | LCDSON;
  LCDBVCTL = LCDCPEN | VLCD_3_44;
  LCDBCTL0 |= LCDON + LCDSON;
  REFCTL0 &= ~REFMSTR;
  
  //Select LCD Segments 0-9
  LCDBPCTL0 = 0xFFFF;
  LCDBPCTL1 = 0xFFFF;

  //LCD Memory
  lcd_zero();
  for(i=0;i<13;i++)
    lcdbm[i]=0xFF;
}

//! Moved the LCD memory to the blink memory, then displays the backup.
void lcd_predraw(){
  //Switch to the backup of the previous frame.
  LCDBMEMCTL |= LCDDISP; // Enable blink memory
}

//! Reverts to the main display.
void lcd_postdraw(){
  //Now swap back the buffer and copy it to blink memory for the next round.
  LCDBMEMCTL &= ~LCDDISP; // Return to main display memory.
  memcpy((char*) lcdbm,(char*) lcdm,13);
}

//! LCD callback when the CPU wakes.
void lcd_wdt(){
  lcd_predraw();

  /* The applet is drawn four times per second.  We handle
     double-buffering, so that incomplete drawings won't be shown to
     the user, but everything else is the app's responsibility. */

  draw_app();

  lcd_postdraw();
}
