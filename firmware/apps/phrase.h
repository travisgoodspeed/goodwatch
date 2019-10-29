/*! \file phrase.h
  \brief Demo passphrase generator app
*/

//! Enter the passphrase tool.
void phrase_init();
//! Exit the passphrase tool.
int phrase_exit();
//! Draw the screen
void phrase_draw();

//! Handle a keypress in the phrase application.
int phrase_keypress(char ch);
