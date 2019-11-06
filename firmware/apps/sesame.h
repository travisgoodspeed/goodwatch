/*! \file sesame.h
  \brief Open Sesame Application
*/


//! Enter the Open Sesame application.
void sesame_init();

//! Exit the Open Sesame application.
int sesame_exit();

//! Draw the Open Sesame screen.
void sesame_draw();

//! Handle an incoming packet.
void sesame_packetrx(uint8_t *packet, int len);

//! Callback after transmission.
void sesame_packettx();

//! Keypress handler for the applet.
int sesame_keypress(char ch);

