/*! \file hex.c
  \brief Hex Viewer
  
  This is a simple viewer application for watching the dmesg buffer.
  Later on, we might add support for beaconing the log over RF.

*/



//MSP430 functions for the SFRIE.
#include <msp430.h>

#include <string.h>

//Include all standard GoodWatch functions.
#include "api.h"

#include "apps/dmesg.h"

//! Our index within the buffer.
static uint16_t dispindex=0;

//! Entry to the hex editor app.
void dmesgapp_init(){
  /* This function is called whenever the app is selected by the Mode
     button.  We don't initialize the address here, because we don't
     want to have it reset, but we do disable some interrupts that
     could trigger a crash on data reads.
  */
  //Force drawing the first frame.
  dmesgapp_draw(1);
  
  return;
}


//! Exit from the hex editor app.
int dmesgapp_exit(){
  /* This function is called whenever the Mode button on the right
     side of the watch is pressed.  If we return 1, it means that our
     own app is using the button, so we return 0 to indicate that the
     task manager is welcome to switch to the next app.
   */
  return 0;
}

//! A button has been pressed for the hex editor.
int dmesgapp_keypress(char ch){
  //Handle the input that we received by an event.
  switch(ch){
  case '7':// Press 7 to transmit the buffer.
    lcd_zero();
    lcd_string("TRANSMIT");
    return 0;  //We'll redraw when the button is released.
  case '.':// Press . to clear the buffer.
    lcd_zero();
    dmesg_clear();
    lcd_string("CLEARED");
    return 0;
  case '0':// Press 0 to return to the start of the buffer.
    dispindex=0;
    break;
  }

  //Force a redraw out-of-frame.
  return 1;
}

//! Draws a bit of the buffer.
static void dmesgapp_drawbuffer(int offset){
  char fragment[8];
  int i;
  
  memcpy(fragment, dmesg_buffer+offset, 8);
  for(i=0;i<8;i++)
    if(fragment[i]>0x7f || !fragment[i])
      fragment[i]=0x20;

  lcd_zero();
  lcd_string(fragment);
}


//! Draw the hex editor app.
void dmesgapp_draw(int forced){
  /* This is called four times per second to render the display.  The
     CPU is running at 32kHz until we tell it otherwise, so it's best
     not to do anything too fancy in a single rendering.
     
     Our screen is double-buffered, so the user won't notice it all
     being drawn.
   */
  if(forced || !key_pressed()){
    //Draw eight bytes.
    dmesgapp_drawbuffer(dispindex++);
    
    //Jump to beginning after we catch up with the current pointer.
    if(dispindex>dmesg_index)
      dispindex=0;
  }
}
