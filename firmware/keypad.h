/*! \file keypad.h
\brief Keypad driver.
*/

//! Scan the keypad.
unsigned int key_scan();
//! Initialize the keypad GPIO pins.
void key_init();
//! Get the currently pressed button as a char.
char getchar();

