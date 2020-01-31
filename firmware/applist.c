/*! \file applist.c
  \brief Table of all active applications.
*/

#include "api.h"
#include "applist.h"

/* These orphaned applets are entered through other means, such as by
   pressing the SET button. */

const struct app setting_applet=
  //Setting time is zero, so that Clock follows.
  {.name="setting", .init=settime_init, .draw=settime_draw, .exit=settime_exit,
   .keypress=settime_keypress
  };
 
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

#ifdef STOPWATCH_APP
  //Stopwatch
  {.name="timer", .init=stopwatch_init, .draw=stopwatch_draw, .exit=stopwatch_exit,
   .keypress=stopwatch_keypress
  },
#endif
  //Submenu selection.
  {.name="submenu", .init=submenu_init,
   .draw=submenu_draw, .keypress=submenu_keypress, .exit=submenu_exit},
  //Selected submenu comes here in sequence, but isn't in the array.
  
  //End on null entry.
  {.name=0, .init=0, .draw=0, .exit=0} 
};

/* Because we had too many applications in the main menu, it became
   necessary to break them out into a submenu.  These behave just like
   the main menu once selected.
 */
const struct app subapps[]={
#ifdef RPN_APP
  //RPN Calculator
  {.name="rpn calc", .init=rpn_init, .draw=rpn_draw, .exit=rpn_exit,
   .keypress=rpn_keypress
  },
#endif


#ifdef ALARM_APP
  //Alarm
  {.name="alarm", .init=alarm_init, .draw=alarm_draw, .exit=alarm_exit,
   .keypress=alarm_keypress
  },
#endif

#ifdef HEX_APP
  //Hex Viewer.
  {.name="hex edit", .init=hex_init, .draw=hex_draw, .exit=hex_exit,
   .keypress=hex_keypress
  },
#endif

#ifdef DMESG_APP
  //DMESG Viewer
  {.name="dmesg", .init=dmesgapp_init, .draw=dmesgapp_draw, .exit=dmesgapp_exit,
   .keypress=dmesgapp_keypress
  },
#endif


#ifdef SHABBAT_APP
  //Kosher applet for Shabbat that disables all inputs except the SET button.
  {.name="shabbat", .init=shabbat_init, .draw=shabbat_draw, .exit=shabbat_exit,
   .keypress=shabbat_keypress
  },
#endif


#ifdef HEBREW_APP
  //Hebrew Calendar applet.  Falls through so that it can run from the clock.
  {.name="hebrew", .fallthrough=hebrew_keypress
   //.init=hebrew_init, .draw=hebrew_draw, .exit=hebrew_exit,
   //.keypress=hebrew_keypress,
   
  },
#endif

#ifdef PHRASE_APP
  // Phrase - passphrase generator
  {.name="phrase", .init=phrase_init, .draw=phrase_draw, .exit=phrase_exit,
   .keypress=phrase_keypress
  },
#endif
#ifdef RNG_APP
  // RNG App - random number generator
  {.name="rng", .init=rngapp_init, .draw=rngapp_draw, .exit=rngapp_exit,
   .keypress=rngapp_keypress
  },
#endif
#ifdef TUNER_APP
  //Tuner Tool
  {.name="tuner", .init=tuner_init, .draw=tuner_draw, .exit=tuner_exit,
   .keypress=tuner_keypress
  },
#endif 
#ifdef COUNTER_APP
  //Counter Tool
  {.name="counter", .init=counter_init, .draw=counter_draw, .exit=counter_exit,
   .keypress=counter_keypress
  },
#endif 
#ifdef MORSE_APP
  //Morse transmitter.
  {.name="morse", .init=morse_init, .draw=morse_draw, .exit=morse_exit,
   .keypress=morse_keypress
  },
#endif
#ifdef BEACON_APP
  //Beacon
  {.name="beacon",
   .init=beacon_init, .draw=beacon_draw, .exit=beacon_exit,
   .packetrx=beacon_packetrx, .keypress=beacon_keypress
  },
#endif
#ifdef OOK_APP
  //OOK
  {.name="OOK",
   .init=ook_init, .draw=ook_draw, .exit=ook_exit,
   .packetrx=ook_packetrx, .packettx=ook_packettx,
   .keypress=ook_keypress
  },
#endif

#ifdef SHADERS_APP
  //somfy RTS
  {.name="shaders",
   .init=shaders_init, .draw=shaders_draw, .exit=shaders_exit,
   .packetrx=shaders_packetrx, .packettx=shaders_packettx,
   .keypress=shaders_keypress
  },
#endif

#ifdef JUKEBOX_APP
  // TouchTunes Jukebox
  {.name="JUKEBOX",
   .init=jukebox_init, .draw=jukebox_draw, .exit=jukebox_exit,
   .packetrx=jukebox_packetrx, .packettx=jukebox_packettx,
   .keypress=jukebox_keypress
  },
#endif


#ifdef PAGER_APP
  //POCSAG Pager
  {.name="pager", .init=pager_init, .draw=pager_draw, .exit=pager_exit,
   .packetrx=pager_packetrx,
   .keypress=pager_keypress
  },
#endif


#ifdef CALIBRATE_APP
  //CALIBRATE
  {.name="calibrate",
   .draw=calibrate_draw, .init=calibrate_init, .exit=calibrate_exit,
   .keypress=calibrate_keypress
  },
#endif
  

  //End on null entry.
  {.name=0, .init=0, .draw=0, .exit=0} 
};

