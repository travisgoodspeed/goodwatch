#include <msp430.h>

#include "keypad.h"

/* P1.5 is the Mode button and P1.6 is the Set button.  We leave them
   in input mode, then briefly check by direct scanning.
   
   Additionally, the buttons need to be emulated on the keypad, as
   they are taken by the serial port on debugging units.  Hold / and *
   for Mode, + and - for Set.
 */

//! Activate the side butons.
void sidebutton_init(){
  //Both are I/O inputs.
  P1SEL&=~(BIT5|BIT6);
  P1DIR&=~(BIT5|BIT6);
  //Pull both up with internal resistors.
  P1REN|=(BIT5|BIT6); 
  P1OUT|=(BIT5|BIT6);
}

//! Test the Mode button.
int sidebutton_mode(){
  if(!(P1IN&BIT5))      //Side button.
    return 1;
  if(key_scan()==0x31)  //Emulation.
    return 1;
  return 0;
}

//! Test the Set button.
int sidebutton_set(){
  if(!(P1IN&BIT6))       //Side button.
    return 1;
  if(key_scan()==0xC1)   //Emulation.
    return 1;
  return 0;
}
