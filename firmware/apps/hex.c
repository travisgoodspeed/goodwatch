/*! \file hex.c
  \brief Hex Viewer
  
  This is a simple hex viewer, allowing the user to explore memory and
  I/O registers.  It also acts as a demo application, which should be
  easy to fork for other purposes.
*/

//MSP430 functions for the SFRIE.
#include <msp430.h>

//Include all standard GoodWatch functions.
#include "api.h"
#include "hex.h"

//! Returns 1 if a given address is legal.
static int hex_islegal(unsigned int adr){
  return !(
	   //(adr>0x1000 && adr<0x17ff) //Bootloader, can be unlocked.

	   //Unknown peripheral; don't know how to unlock this region.
	   (adr>=0x1b00 && adr<0x1c00)
	   );
}

static int hex_can_edit(unsigned int adr) {
  return !(
    (adr >= 0x1000 && adr <= 0x17ff) || // Bootloader

    // Unknown peripheral; don't know how to unlock this region.
    (adr >= 0x1b00 && adr < 0x1c00) ||

    (adr >= 0x8000) || // Flash Memory
    (adr >= 0x1800 && adr < 0x19ff) // Info flash
  );
}


//! Entry to the hex editor app.
void hex_init(){
  /* This function is called whenever the app is selected by the Mode
     button.  We don't initialize the address here, because we don't
     want to have it reset, but we do disable some interrupts that
     could trigger a crash on data reads.
  */
  
  //disable interrupts for unmapped peripheral violations.
  SFRIE1&=~(VMAIE|ACCVIE);
  //Disable BSL protections, so we can read it without trouble.
  SYSBSLC=3;

  //Force drawing the first frame.
  hex_draw(1);
  
  return;
}

//! Address of the hex editor.
static int adr=0x8000; //Beginning of Flash.

//! If non-zero, we are editing the memory; value is the digit being edited.
static int editing = 0;

static unsigned int new_val = 0;

//! Exit from the hex editor app.
int hex_exit(){
  /* This function is called whenever the Mode button on the right
     side of the watch is pressed.  If we return 1, it means that our
     own app is using the button, so we return 0 to indicate that the
     task manager is welcome to switch to the next app.
   */
  if (editing) {
    editing++;
    return 1;
  }
  return 0;
}


//! A button has been pressed for the hex editor.
int hex_keypress(char ch){
  //Handle the input that we received by an event.
  if (editing) {
    // Convert keypress to a nibble
    if (ch == 0) {
      return 0;
    }
    // A non-numeric key has been pressed
    char inputdigit;
    switch (ch) {
      case '/':
        inputdigit = 0x0A;
      break;
      case '*':
        inputdigit = 0x0B;
      break;
      case '-':
        inputdigit = 0x0C;
      break;
      case '+':
        inputdigit = 0x0D;
      break;
      case '=':
        inputdigit = 0x0E;
      break;
      case '.':
        inputdigit = 0x0F;
      break;
      default: // One of the (0-9) digits was pressed
        inputdigit = ch & 0x0F;
    }

    char shift = ((4 - editing) * 4);
    new_val = (new_val & ~(0xF << shift)) | (inputdigit << shift);
    editing++;
  } else {
    switch(ch){
    case '4':// Hold 4 to disassemble the curent instruction.
      lcd_zero();
      asm_dis(adr,
          ((unsigned int*)adr)[0],
          ((unsigned int*)adr)[1],
          ((unsigned int*)adr)[2]);
      asm_show();
      return 0;  //We'll redraw when the button is released.
      
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
  }

  //Force a redraw out-of-frame.
  return 1;
}


//! Draw the hex editor app.
void hex_draw(int forced){
  /* This is called four times per second to render the display.  The
     CPU is running at 32kHz until we tell it otherwise, so it's best
     not to do anything too fancy in a single rendering.
     
     Our screen is double-buffered, so the user won't notice it all
     being drawn.
   */
  static int flicker = 0;

  // The sidebutton needs to be polled for a change
  if (sidebutton_set()) {
    editing = !editing;
    if (editing) {
      if(!hex_can_edit(adr)){
        // do not allow editing of memory regions that do not support editing
        editing = 0;
      } else {
        // Initialize the new value to the value currently in memory
        new_val = ((unsigned int*)adr)[0];
      }
    } else {
      forced = 1; // The screen needs to be redrawn. Cancelling editing using
      // the set button will not trigger a redraw, so we do it manually.
    }
  }

  // editing mode draws every cycle to flash the nibble currently being input
  if (editing) {
    // Editing mode needs to handle input from the mode button and the keypad
    // because of this, any updates that can be triggered by the mode button
    // and the keypad are handled here. 
    lcd_zero();
    lcd_hex(
      (((unsigned long)adr) << 16) // Address
      | new_val //data
    );
    setperiod(4, 1);

    if (editing <= 4) {
      flicker = !flicker;
      if (flicker) {
        lcd_cleardigit(4 - editing);
      }
    } else {
      // Editing is > 4, so one word of data has been input. Write the value
      // to ram and exit editing mode
      ((unsigned int*)adr)[0] = new_val;
      editing = 0;
      flicker = 0; // Probably not necessary, but nice to reset state
    }
  } else if(forced) {
    //And finally we draw the result to the screen.
    lcd_zero();
    
    /* The bootloader range is illegal to read, so we skip it.  In the
       future, we might bypass this by reflecting our request through
       the bootloader region, or trying to find the protection flag that
       is keeping us from accessing it.
    */
    if(!hex_islegal(adr)){
      lcd_hex(
              (((unsigned long)adr)<<16) // Address
              );
      lcd_cleardigit(3);
      lcd_cleardigit(2);
      lcd_cleardigit(1);
      lcd_cleardigit(0);
      return;
    }else{
      /* Now that the range is legal and the value known, let's fetch the
         value and display it.
      */
      lcd_hex(
              (((unsigned long)adr)<<16) // Address
              | ((unsigned int*)adr)[0] //data
              );
    }
  }

}
