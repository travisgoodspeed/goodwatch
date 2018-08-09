/*! \file shabbat.c
  \brief Shabbat Application
  
  This applet places the watch into a "Shabbat Mode", in which all
  accidental keypresses are ignored.  Further, the GPIO pins are
  placed into a configuration in which no transistors will flip if a
  button is accidentally pressed.
  
  Enter the applet by choosing it in the submenu.  To exit this
  applet, you must press the recessed SET sidebutton, as all other
  buttons are ignored.  The drawing function will monitor that
  interrupts and GPIO pins remain in a kosher state and display an
  error if they have accidentally changed.
  
*/

#include <stdio.h>
#include <msp430.h>
#include "apps/clock.h"
#include "api.h"

//When non-zero, we redraw the entire time.
static int redraw=0;

//! Enter shabbat mode.
static void enter_shabbat(){
  /* For the watch to be kosher in this mode, it is not sufficient to
     simply ignore the keypresses; rather, we must also ensure that
     accidental keypresses will cause no transistors to switch or code
     to be executed.  This is accomplished by moving all the keypad
     GPIO pins into output mode, and all of them low.  Futher, the
     MODE button must also be switched to output mode, as it can be
     accidentally pressed from habit.

     The SET sidebutton, which is resessed and never pressed out of
     habit, will remain the sole method of input to the watch
     hardware.  Pressing that button will exit Shabbat Mode and return
     the watch to normal operation.
   */

  /* This is the key matrix.  All of these pins must be off and low
     voltage.

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

  //Disable interrupts.
  P2IE=0;
  
  /* All columns and all rows need to be output mode and low. */
  P2SEL&=~0x7F; //All of port 2 except the buzzer are IO.
  P2OUT = 0x00; //All pins are low.
  P2REN = 0x00; //No pulling resistors.
  P2DIR = 0x7F; //All pins are output.
  
  P1OUT&=~0x80; //Output low.
  P1SEL&=~0x80; //P1.7 is IO.
  P1DIR|= 0x80; //P1.7 is output.
  P1REN&=~0x80; //No pulling resistor.

  /* The Mode (P1.5) button also needs to be disabled, but we leave
     Set (P1.6) alone because it is unlikely to be accidentally
     pressed.
   */
  P1OUT&=~BIT5; //Low output.
  P1DIR|= BIT5; //Output mode.
  P1REN&=~BIT5; //Disable resistor.
}

//! Returns non-zero if the keypad or Mode button have any inputs.
static int test_shabbat(){
  if(P2IE             //Port 2 interrupts must be off.
     || !(P2DIR&0x7F) //Port 2 keys must be outputs.
     || !(P1DIR&BIT7) //P1.7 must be output.
     || !(P1DIR&BIT5) //Mode button must be output
     ) return 1;

  //All good.
  return 0;
}

//! Exit to normal mode.
static void exit_shabbat(){
  //Return keypad to normal.
  key_init();
  //Return sidebuttons to normal.
  sidebutton_init();
}

//! Enter the Shabbat application.
void shabbat_init(){
  //Draw the entire time on entry.
  redraw=1;
  //Enter shabbat mode, so that the keypad is disabled.
  enter_shabbat();
}

//! Exit the Shabbat application.
int shabbat_exit(){
  //Return GPIO to normal, just to be safe.
  exit_shabbat();
  //Good to move to the next applet.
  return 0;
}

//! Draw the Shabbat screen.
void shabbat_draw(){
  //Use the SET button to exit Shabbat mode.
  if(sidebutton_set()){
    //Wait for the button to be released.
    while(sidebutton_set());
    //Return GPIO to normal, which should show the PANIC message.
    exit_shabbat();
  }

  
  /* Display a strong warning if we aren't in shabbat mode. */
  if(test_shabbat()){
    lcd_string("PANIC   ");
    return;
  }
    
  /* We draw the time with the same redraw behavior as the standard
     clock applet. */
  draw_time(redraw);
  if(redraw)
    redraw--;

  /* Like the stopwatch, the shabbat applet will not timeout after
     three minutes.  This is dangerous to do unless your applet is
     just as power efficient as the clock.
   */
  app_cleartimer();
}

//! Keypress handler for the shabbat applet.
int shabbat_keypress(char ch){
  /* It might seem silly to have a keypress handler in an applet whose
     sole purpose is to turn off the keypad, but here it serves as a
     backup, to ensure that changes to the operating system as a whole
     haven't invalidated the behavior of this applet.
   */
  lcd_string("Uh oh.");
  return 0;
}

