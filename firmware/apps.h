/* 
   These are functions for working with applications and the
   user-visible task switcher.

 */

#include "apps/clock.h"
#include "apps/rpn.h"


struct app {
  void (*init)(void); //Called exactly once at startup.
  void (*draw)(void); //Called four times per second to draw display.
  void (*exit)(void); //Called once when moving to the next applet.
};



void draw_app();
