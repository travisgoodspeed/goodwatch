/* 
   Each of these function pointers is called four times per second.
 */
static struct app apps[]={
  //Clock
  {.init=clock_init, .draw=clock_draw, .exit=clock_exit},
  //RPN Calculator
  {.init=rpn_init, .draw=rpn_draw, .exit=rpn_exit},

  //End on null entry.
  {.init=0, .draw=0, .exit=0} 
};
