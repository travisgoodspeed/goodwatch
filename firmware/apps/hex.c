/* This is a simple hex viewer, allowing the user to explore memory
   and I/O registers.  It also acts asa a demo application, which
   should be easy to fork for other purposes.
*/


//Include all standard GoodWatch functions.
#include "api.h"

//! Entry to the hex editor app.
void hex_init(){
  /* This function is called whenever the app is selected by the Mode
     button, but we don't want to abandon our place, so we don't
     actually initialize anything here.
   */
  return;
}


//! Exit from the hex editor app.
int hex_exit(){
  /* This function is called whenever the Mode button on the right
     side of the watch is pressed.  If we return 1, it means that our
     own app is using the button, so we return 0 to indicate that the
     task manager is welcome to switch to the next app.
   */
  return 0;
}

//! Draw the hex editor app.
void hex_draw(){
  /* This is called four times per second to render the display.  The
     CPU is running at 32kHz until we tell it otherwise, so it's best
     not to do anything too fancy in a single rendering.
     
     Our screen is double-buffered, so the user won't notice it all
     being drawn.
   */
  
  static int adr=0x8000; //Beginning of Flash.
  char ch=getchar();

  //Do nothing if no key is pressed.
  //if(!ch)
  // return;

  //Handle the input that we sampled above.
  switch(ch){
    /* The top button on each column increases that nybble of the address.
       The bottom button reduces it.
    */
  case '7':
    adr+=0x1000;
    break;
  case '0':
    adr-=0x1000;
    break;
  case '8':
    adr+=0x0100;
    break;
  case '.':
    adr-=0x0100;
    break;
  case '9':
    adr+=0x0010;
    break;
  case '=':
    adr-=0x0010;
    break;
  case '/':
    adr+=2;
    break;
  case '+':
    adr-=2;
    break;
  }

  //And finally we draw the result to the screen.

  
  lcd_zero();

  /* The bootloader range is illegal to read, so we skip it.  In the
     future, we might bypass this by reflecting our request through
     the bootloader region, or trying to find the protection flag that
     is keeping us from accessing it.
   */
  if(adr>0x1000 && adr<=0x17FF){
    lcd_hex(
	    (((unsigned long)adr)<<16) // Address
	    | 0xdead //data
	    );
    return;
  }
  
  /* Now that the range is legal and the value known, let's fetch the
     value and display it.
   */
  lcd_hex(
	  (((unsigned long)adr)<<16) // Address
	  | ((unsigned int*)adr)[0] //data
	  );
}
