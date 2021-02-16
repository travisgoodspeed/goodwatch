/*! \file beats.h
  \brief Beats Application
  
*/

//! Initialize the beats.
void beats_init();
//! Exit to the next application.
int beats_exit();
//! Draw the beats.
void beats_draw();

//! A button has been pressed for the beats.
int beats_keypress(char ch);

//! A button has been pressed in fallthrough mode
int beats_fallthrough_keypress(char ch);