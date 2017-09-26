/* Minimal LCD example, forked from a reference by G. Larmore at TI.
   The LCD pinout doesn't match ours yet, and will need to be
   modified to fit.  */

#include <msp430.h>
#include <string.h>

#include "lcdtext.h"
#include "rtc.h"
#include "apps.h"
#include "sidebutton.h"

volatile unsigned char *lcdm=&LCDM1;
volatile unsigned char *lcdbm=&LCDBM1;

//! Clears the LCD memory and blink memory.
void lcd_zero(){
  LCDBMEMCTL|=LCDCLRM;
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
  //Mark some flags no matter what the mode.
  if((UCSCTL4&SELM_7)!=SELM_0)
    setmult(1);  //Mult indicates main clock is not from XT1
  if(key_pressed())
    setdivide(1);
  if(sidebutton_mode())
    setplus(1);
  
  //Now swap back the buffer.
  LCDBMEMCTL &= ~LCDDISP; // Return to main display memory.

  //Copy to blink memory for the next round.
  memcpy((char*) lcdbm,(char*) lcdm,13);


  /* Uncomment this for a sort of CPU monitor, which will darken one
     piece of the day-of-week characters only while the rest of the
     screen is being drawn.  If the segment is very dark, you might be
     taking more than your fair share of cycles.  If it is very light,
     you are either not redrawing the screen, or you have plenty of
     cycles to spare.
   */
  //lcdbm[0x0c]|=0x01; //Set a segment to visualize delay times.
}

//! LCD callback when the CPU wakes.
void lcd_wdt(){
  lcd_predraw();

  /* The applet is drawn four times per second.  We handle
     double-buffering, so that incomplete drawings won't be shown to
     the user, but everything else is the app's responsibility. */

  app_draw();

  lcd_postdraw();
}
