/*! \file morse.h
  \brief Handy morse code tool for 70cm.

*/

//! Enter the Morse app.
void morse_init();
//! Exit the Morse app.
int morse_exit();
//! Draw the screen.
void morse_draw();

//! A button has been pressed for Morse.
int morse_keypress(char ch);
