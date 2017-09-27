#include "apps/clock.h"
#include "apps/stopwatch.h"
#include "apps/rpn.h"
#include "apps/hex.h"


/* 
   Each of these function pointers is called four times per second.
 */
static struct app apps[]={
  //Clock
  {.init=clock_init, .draw=clock_draw, .exit=clock_exit},
  //Stopwatch
  {.init=stopwatch_init, .draw=stopwatch_draw, .exit=stopwatch_exit},

  //RPN Calculator
  {.init=rpn_init, .draw=rpn_draw, .exit=rpn_exit},
  //Hex Viewer.
  {.init=hex_init, .draw=hex_draw, .exit=hex_exit},
  
  //End on null entry.
  {.init=0, .draw=0, .exit=0} 
};
