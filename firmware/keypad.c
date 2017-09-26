#include <msp430.h>

#include "apps.h"

//Initializes the keypad pin directions.
void key_init(){
  /* The keypad uses P1.7 and all of Port 2.  Pins are bridged
     together by rows and columns, which requires active scanning for
     a read but can allow for an initial wakeup on an interrupt. */

  P2SEL=0x00;  //All of port 2 are IO.
  P2DIR=0x00;  //All are input.
  P2REN|=0xFF; //All resistor.
  P2OUT=0x00; //Pull all of them down.

  P1SEL&=~0x80; //P1.7 is IO.
  P1DIR&=~0x80; //P1.7 is input.
  P1REN|=0x80;
  P1OUT&=~0x80;  //Output low.
}

//! Bitfields indicate pressed rows.
int key_row(){
  int row;
  P1DIR|=0x80; //P1.7 out.
  P1OUT|=0x80; //P1.7 high.
  P2DIR= 0x07;  //P2.0, 2.1, 2.2 out.
  P2OUT= 0x07;  //P2.0, 2.1, 2.2 high.

  //We'll return this result, but after cleaning up.
  row=(P2IN>>3)&0x0F;

  
  P1DIR&=~0x80; //All input.
  P2DIR=0x00; 
  P1OUT&=~0x80; //All down.
  P2OUT=0x00; 
  
  return row;
}

//! Bitfields indicate pressed columns.
int key_col(){
  int col;
  P1DIR&=~0x80; //Input
  P1OUT&=~0x80; //Low
  P2DIR= 0xF8;  //P2.1.3, 2.4, 2.5, 2.6 out
  P2OUT= 0xF8;  //P2.1.3, 2.4, 2.5, 2.6 high

  //We'll return this result, but after cleaning up.
  col=((P2IN&0x7)<<1) | ((P2IN&0x80)>>7);

  P1DIR&=~0x80; //All input.
  P2DIR=0x00; 
  P1OUT&=~0x80; //All down.
  P2OUT=0x00;

  //Temporary workaround for what I think is hardware damage in my
  //prototype.
  if(col&~1)
    return col&0xFE;

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

  unsigned int scan=(key_row()<<4)|key_col();

  if(scan&0xF0)
    return scan;
  else
    return 0;
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
unsigned int key_chr(code){
  /* Row is the upper nybble, column is the lower nybble.*/
  unsigned int i=0;
  for(i=0;keymap[i];i++){
    if(keymap[i]>>8==code)
      return keymap[i]&0xFF;
  }
  
  //Return null for all unknown codes.
  return 00;
}

//! Gets a character as ASCII.
char getchar(){
  char c=key_chr(key_scan());
  if(c)
    //Clear the app timer when a button is pressed.
    app_cleartimer();
  return c;
}
