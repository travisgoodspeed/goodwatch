/*! \file keypad.c
  \brief Keypad driver.
 */

#include <stdint.h>
#include <stdio.h>
#include <msp430.h>

#include "apps.h"

static void setdirections(){
  /* Columns 2.2, 2.1, 2.0, and 1.7 are set to output mode pulled
     high, so that any low row indicates a button press. */
  P2SEL&=~0x7F;  //All of port 2 except the buzzer are IO.
  P2DIR=BIT2|BIT1|BIT0;  //These columns are  output.
  P2REN|=0x7F; //All resistor.
  P2OUT=BIT2|BIT1|BIT0; //Pull columns up.

  P1SEL&=~0x80; //P1.7 is IO.
  P1DIR|=0x80;  //P1.7 is output.
  P1REN|=0x80;
  P1OUT|=0x80;  //Output high.
}

//! Initializes the keypad pin directions.
void key_init(){
  /* The keypad uses P1.7 and all of Port 2.  Pins are bridged
     together by rows and columns, which requires active scanning for
     a read but can allow for an initial wakeup on an interrupt. */

  //Disable interrupts first.
  P2IE=0;

  setdirections();

  P2DIR|=BIT7; // Buzzer is on P2.7.

  //Trigger interrupts for keypresses so we needn't scan.
  P2IE=BIT3|BIT4|BIT5|BIT6;
}

//! Quickly checks to see if a key is pressed.
int key_pressed(){
  return (P2IN&(BIT3|BIT4|BIT5|BIT6));
}

//! Bitfields indicate pressed rows.
int key_row(){
  int row;

  /* Already the default directions when this is called, but uncomment
     if used elsewhere.

  P1DIR|=0x80; //P1.7 out.
  P1OUT|=0x80; //P1.7 high.
  P2DIR|=0x07;  //P2.0, 2.1, 2.2 out.
  P2OUT|=0x07;  //P2.0, 2.1, 2.2 high.
  */

  //We'll return this result, but after cleaning up.
  row=(P2IN>>3)&0x0F;
  
  return row;
}

//! Bitfields indicate pressed columns.
int key_col(){
  int col;
  P1DIR&=~0x80; //Input
  P1OUT&=~0x80; //Low

  P2DIR&=~0x07; //P2.0, 2.1, 2.2 in
  P2OUT&=~0x07; //P2.0, 2.1, 2.2 low.
  P2OUT|= 0x78; //P2.3, 2.4, 2.5, 2.6 high
  P2DIR|= 0x78; //P2.3, 2.4, 2.5, 2.6 out

  //We'll return this result, but after cleaning up.
  col=((P2IN&0x7)<<1) | ((P1IN&0x80)>>7);
  
  return col;
}


//! Returns the key code of the currently pressed button.
unsigned int key_scan(){
  /* This is the key matrix.  P2.7 isn't used, which would've been
     nice to know earlier.

      2.2  2.1  2.0  1.7
       |    |    |    |
2.3----+----+----+----+--10
       |    |    |    |
2.4----+----+----+----+--20
       |    |    |    |
2.5----+----+----+----+--40
       |    |    |    |
2.6----+----+----+----+--80
       |    |    |    |
      08   04   02   01
  */

  unsigned int scan=0;
  
  //When no buttons are pressed, we need to shortcut the scan to avoid
  //wasting I/O and energy.
  if(!key_pressed())
    return 0;
  
  //Some button is pressed, so do an active scan to figure out which
  //one.
  scan=(key_row()<<4)|key_col();
  
  setdirections();
  
  if(scan&0xF0)
    return scan;
  else{
    printf("%02x is an incomplete scancode.\n",scan);
    return 0;
  }
}


//! Map of presses to buttons.
const unsigned int keymap[]={
  /* Upper nybble is the scan code, lower nybble is the byte in ASCII.
   */
  0x1837, 0x1438,     0x1239,     0x1100|'/',
  0x2834, 0x2435,     0x2236,     0x2100|'*',
  0x4831, 0x4432,     0x4233,     0x4100|'-',
  0x8830, 0x8400|'.', 0x8200|'=', 0x8100|'+',
  0x0000 //End on a null byte.
};

//! Returns a character of the current scan code.
unsigned int key_chr(int code){
  /* Row is the upper nybble, column is the lower nybble.*/
  unsigned int i=0;
  for(i=0;keymap[i];i++){
    if(keymap[i]>>8==code)
      return keymap[i]&0xFF;
  }
  
  //Return null for all unknown codes.
  return 00;
}


//! Gets the currently held button as ASCII.  Don't use for typing.
char key_char(){
  char c=key_chr(key_scan());
  if(c)
    //Clear the app timer when a button is pressed.
    app_cleartimer();
  return c;
}

//! Interrupt handler for Port2.
void __attribute__ ((interrupt(PORT2_VECTOR))) PORT2_ISR(void){
  static char lastchar=0x00;
  char newchar;

  //We used to clear P2IFG here, but it would cause the interrupt to
  //be re-triggered until the button was released, wasting a lot of
  //power now that the CPU clock is fast.
  //P2IFG=0;

  //We must also configure the edge triggering such that we get
  //another interrupt on the *change* rather than just because a
  //button is still held.
  P2IES=P2IN&(BIT3|BIT4|BIT5|BIT6);
  //P2IFG=0;

  //Scan for the new character, which we will compare to the old to
  //reduce duplicates.
  newchar=key_chr(key_scan());

  //Clear the flag and restore all the scanning to defaults.
  setdirections();
  P2IFG=0;

  /* By this point in the handler, we're done with handling the key
     interrupt, so the flag has been cleared.  When we pass control to
     the handler, it might take an absurdly long time to return, but
     as long as the keypad change happens as it is being run, control
     will come right back to this ISR on the next go round.
   */
  

  //Bail quickly when the key is the same.
  if(lastchar!=newchar){
    lastchar=newchar;
    app_cleartimer(); //Clear the idle timer.
    
    app_keypress(newchar);
  }else{
    //printf(".");
  }
}

