/*! \file lcd.c
  \brief LCD Driver.
  
  Minimal LCD library, based on a handy reference by G. Larmore at
  Texas Instruments.  See lcdtext.c for the font library.
*/

#include <msp430.h>
#include <string.h>
#include <stdio.h>

#include "api.h"

//! LCD Main memory.
volatile unsigned char *lcdm=&LCDM1;
//! LCD Blink Memory, for double-buffering.
volatile unsigned char *lcdbm=&LCDBM1;

//! While non-zero, this blinks a useless LCD segment to indicate CPU load.
int flickermode=0;

//! Clears the LCD memory and blink memory.
void lcd_zero(){
  LCDBMEMCTL|=LCDCLRM;
}

//! Initialize the LCD memory and populate it with sample text.
void lcd_init() {
  int i;


  LCDBCTL0 = 0;
  
  // Select LCD COM pins
  P5SEL |= (/*BIT5 |*/ BIT6 | BIT7);
  P5DIR |= (/*BIT5 |*/ BIT6 | BIT7);

  /* This should keep the LCD charge pump on even when we are using
     the radio, but for some reason, it doesn't work.  See issue #56
     on the tracker.
   */
  LCDBCPCTL=0;


  // Configure LCD_B
  // LCD_FREQ = ACLK/32/4, LCD Mux 3, turn on LCD
  // Charge pump generated internally at 3.44V, external bias (V2-V4) generation
  // Internal reference for charge pump
  LCDBCTL0 =  (LCDDIV0 | LCDDIV1 | LCDDIV2 | LCDDIV3 | LCDDIV4)
    | LCDPRE0
    | LCD3MUX //Fourth 
    //| LCDSSEL //Use the VLO clock instead of ACLK.  (Needs different dividers.)
    ;
  LCDBVCTL = LCDCPEN | VLCD_3_44 | LCD2B; //Highest contrast, 1/2 BIAS
  //LCDBVCTL = LCDCPEN | VLCD_3_02 | LCD2B; //Mid contrast
  //LCDBVCTL = LCDCPEN | VLCD_2_60 | LCD2B; //Weakest contrast.
  LCDBCTL0 |= LCDON + LCDSON;
  REFCTL0 &= ~REFMSTR;// //Disable legacy mode.
  
  //Select LCD Segments 0-9
  LCDBPCTL0 = 0xFFFF;
  LCDBPCTL1 = 0xFFFF;


  //Begin by blacking the whole display, for diagnostics if our clocks
  //fail.
  for(i=0;i<13;i++){
    lcdm[i]=0xFF;
    lcdbm[i]=0xFF;
  }
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
    setmult(1);     //Mult indicates main clock is not from XT1
  if(UCSCTL7&2){
    setdivide(1);   //Div indicates a crystal fault.
    printf("Clock fault, attempting repair.\n");
    ucs_init();

    if(UCSCTL7&2){
      printf("Didn't work.\n");
    }else{
      printf("Success!\n");
    }
  }
  if(power_ishigh())
    setminus(1);    //Minus indicates the radio is on.

  // display marquee, if active
  draw_marquee();
  
  //Now swap back the buffer.
  LCDBMEMCTL &= ~LCDDISP; // Return to main display memory.

  //Copy to blink memory for the next round.
  memcpy((char*) lcdbm,(char*) lcdm,13);

  /* This is a sort of CPU monitor, which will darken one
     piece of the day-of-week characters only while the rest of the
     screen is being drawn.  If the segment is very dark, you might be
     taking more than your fair share of cycles.  If it is very light,
     you are either not redrawing the screen, or you have plenty of
     cycles to spare.
     
     Start it by pressing 6 in clock mode.
   */
  if(flickermode){
    flickermode--;
    lcdbm[0x0c]|=0x01; //Set a segment to visualize delay times.
  }
}

