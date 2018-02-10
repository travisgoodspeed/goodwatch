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

  /* I/O can work either by interrupts or by polling.  The old
     convention was to call getchar() to poll during the rendering
     loop if you are checking to see if a button is held down, but to
     take this interrupt callback when taking number entry, such as in
     the calculator.
     
     Called once per unique keypress.  Return non-zero to immediately redraw.
  */
  int (*keypress)(char ch);//A keypress has arrived.

  /* Callbacks for packets being sent and received.  Set to null if unused. */
  void (*packetrx)(uint8_t *packet, int len); //A packet has arrived.
  void (*packettx)(void); //A packet has been sent.
};


//! Draw the current application.
void app_draw();
//! Initializes the set of applications.
void app_init();
//! Move to the next application.
void app_next();
//! If this is *not* called, the watch forces home after three minutes.
void app_cleartimer();
//! Drop the power usage and return to the Clock.
void app_forcehome();
//! Provide an incoming packet.
void app_packetrx(uint8_t *packet, int len);

//! Handles a keypress, if a handler is registered.
void app_keypress(char ch);

//! Sets an app by a pointer to its structure.  Used for submenus.
void app_set(const struct app *newapplet);
