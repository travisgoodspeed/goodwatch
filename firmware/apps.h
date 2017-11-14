/*! \file apps.h
  
  \brief These are functions for working with applications and the task switcher.

 */


//! Entry in the application table.
struct app {
  char *name;         //Shows when entering the app.
  void (*init)(void); //Called exactly once at startup.
  void (*draw)(void); //Called four times per second to draw display.
  
  /* Called once when moving to the next applet.  Returns zero (or is
     NULL) if the application may move on, or returns non-zero if the
     exit call is intercepted.
   */
  int (*exit)(void); 
};


//! Draw the current application.
void app_draw();
//! Initialize the current application.
void app_init();
//! Move to the next application.
void app_next();
//! If this is *not* called, the watch forces home after three minutes.
void app_cleartimer();
//! Drop the power usage and return to the Clock.
void app_forcehome();
