#include <msp430.h>

#include "apps.h"
#include "lcdtext.h"

#include "applist.h"

//! We begin on the clock.
static int appindex=0, idlecount=0;

//! Every 3 minutes we return to the clock unless this is called.
void app_cleartimer(){
  idlecount=0;
}

//! Renders the current app to the screen.
void app_draw(){
  static int lastmin=0;;
  
  void (*tocall)(void)=apps[appindex].draw;

  //If we go three minutes without action, return to main screen.
  if(lastmin!=RTCMIN){
    lastmin=RTCMIN;
    idlecount++;
  }
  if(idlecount>3){
    app_cleartimer();
    appindex=0;
  }
  
  //Call the cap if it exists, or switch to the clock if we're at the
  //end of the list.
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
  
  //Clear the 3-minute timer when we switch apps.
  app_cleartimer();
  
  if(!tocall)
    appindex=0;
  
  return;
}
