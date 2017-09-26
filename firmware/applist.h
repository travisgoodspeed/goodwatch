/* 
   Each of these function pointers is called four times per second.
 */
static struct app apps[]={
  {.init=0, .draw=clock_draw, .exit=0}, //Clock
  {.init=0, .draw=rpn_draw, .exit=0},   //RPN Calculator
  {.init=0, .draw=0, .exit=0} //End on null entry.
};
