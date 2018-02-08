/*! \file hex.h
  \brief Hex editor application.
*/

//! Enter the hex editor.
void hex_init();
//! Exit the hex editor.
int hex_exit();
//! Draw the screen.
void hex_draw();

//! A button has been pressed for the hex editor.
int hex_keypress(char ch);
