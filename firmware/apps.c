#include <msp430.h>

#include "apps.h"
#include "lcdtext.h"

#include "applist.h"

int appindex=0;

//! Renders the current app to the screen.
void draw_app(){
  void (*tocall)(void)=apps[appindex].draw;
  if(tocall)
    tocall();
  else
    appindex=0;
  return;
}

//Handles 
