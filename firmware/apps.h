/* 
   These are functions for working with applications and the
   user-visible task switcher.

 */



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



void app_draw();
void app_init();
void app_next();
void app_cleartimer();
void app_forcehome();
