/* 
   Each of these function pointers is called four times per second.
 */
static struct app apps[]={
  {.init=clock_init, .draw=clock_draw, .exit=0}, //Clock
  {.init=rpn_init, .draw=rpn_draw, .exit=0},   //RPN Calculator
  {.init=0, .draw=0, .exit=0} //End on null entry.
};
