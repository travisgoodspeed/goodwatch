/*! \file applist.c
  \brief Table of all active applications.
*/

#include "api.h"
#include "applist.h"

/* For each application, the init() function is called at entry.  The
   draw() function is called four times per second.  The exit()
   function is called when the mode button is pressed, but returns 1
   to reject a mode switch or 0 to allow it.
 */
const struct app apps[]={
  //Clock
  {.name="clock", .init=clock_init, .draw=clock_draw, .exit=clock_exit,
   .keypress=clock_keypress
  },
  //Alarm
  {.name="alarm", .init=alarm_init, .draw=alarm_draw, .exit=alarm_exit,
   .keypress=alarm_keypress
  },
  //Stopwatch
  {.name="timer", .init=stopwatch_init, .draw=stopwatch_draw, .exit=stopwatch_exit,
   .keypress=stopwatch_keypress
  },
  //RPN Calculator
  {.name="rpn calc", .init=rpn_init, .draw=rpn_draw, .exit=rpn_exit,
   .keypress=rpn_keypress
  },

  //Submenu selection.
  {.name="submenu", .draw=submenu_draw, .keypress=submenu_keypress, .exit=submenu_exit},
  //Selected submenu comes here in sequence, but isn't in the array.
  
  //End on null entry.
  {.name=0, .init=0, .draw=0, .exit=0} 
};

/* Because we had too many applications in the main menu, it became
   necessary to break them out into a submenu.  These behave just like
   the main menu once selected.
 */
const struct app subapps[]={
  //Hex Viewer.
  {.name="hex edit", .init=hex_init, .draw=hex_draw, .exit=hex_exit,
   .keypress=hex_keypress
  },
  // Diceware - passphrase generator
  {.name="diceware", .init=dice_init, .draw=dice_draw, .exit=dice_exit,
   .keypress=dice_keypress
  },
  // RNG App - random number generator
  {.name="rng", .init=rngapp_init, .draw=rngapp_draw, .exit=rngapp_exit,
   .keypress=rngapp_keypress
  },
  //Tuner Tool
  {.name="tuner", .init=tuner_init, .draw=tuner_draw, .exit=tuner_exit,
   .keypress=tuner_keypress
  },
  //Morse transmitter.
  {.name="morse", .init=morse_init, .draw=morse_draw, .exit=morse_exit},

  //Beacon
  {.name="beacon",
   .init=beacon_init, .draw=beacon_draw, .exit=beacon_exit,
   .packetrx=beacon_packetrx, .keypress=beacon_keypress
  },

  //iClicker
  //{.name="iclicker", .init=iclicker_init, .draw=iclicker_draw, .exit=iclicker_exit},
  
  //End on null entry.
  {.name=0, .init=0, .draw=0, .exit=0} 
};

