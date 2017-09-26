#include <msp430.h>

#include "apps.h"
#include "lcdtext.h"

#include "applist.h"

//! We begin on the clock.
static int appindex=0;

//! Renders the current app to the screen.
void app_draw(){
  void (*tocall)(void)=apps[appindex].draw;
  if(tocall)
    tocall();
  else
    appindex=0;
  return;
}

//! Renders the current app to the screen.
void app_init(){
  void (*tocall)(void)=apps[appindex].init;
  if(tocall)
    tocall();
  else
    appindex=0;
  return;
}

//! Move to the next application.
void app_next(){
  void (*tocall)(void)=apps[++appindex].draw;
  if(!tocall)
    appindex=0;
  
  return;
}
