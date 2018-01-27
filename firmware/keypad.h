/*! \file keypad.h
\brief Keypad driver.
*/

//! Scan the keypad.
unsigned int key_scan();
//! Initialize the keypad GPIO pins.
void key_init();
//! Gets the currently held button as ASCII.  Don't use for typing.
char getchar();

