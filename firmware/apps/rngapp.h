/*! \file rngapp.h
  \brief Numeric random number generator applet
*/

//! Enter the rngapp tool.
void rngapp_init();

//! Exit the rngapp tool.
int rngapp_exit();

//! Draw the screen
void rngapp_draw();

//! Handle a keypress in the rngapp application.
int rngapp_keypress(char ch);
