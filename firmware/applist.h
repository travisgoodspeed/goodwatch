/*! \file applist.h
  \brief Table of all active applications.  Add yours here.
*/

#include "apps/clock.h"
#include "apps/stopwatch.h"
#include "apps/rpn.h"
#include "apps/hex.h"


/*  For each application, the init() function is called at entry.  The
    draw() function is called 4 times per second.  The exit() function
    is called when the mode button is pressed, but returns 1 to reject
    a mode switch or 0 to allow it.
 */
static struct app apps[]={
  //Clock
  {.name="clock", .init=clock_init, .draw=clock_draw, .exit=clock_exit},
  //Stopwatch
  {.name="timer", .init=stopwatch_init, .draw=stopwatch_draw, .exit=stopwatch_exit},

  //RPN Calculator
  {.name="rpn calc", .init=rpn_init, .draw=rpn_draw, .exit=rpn_exit},
  //Hex Viewer.
  {.name="memory", .init=hex_init, .draw=hex_draw, .exit=hex_exit},
  
  //End on null entry.
  {.name=0, .init=0, .draw=0, .exit=0} 
};
