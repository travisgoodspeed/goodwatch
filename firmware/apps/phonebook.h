/*! \file phonebook.h
  \brief Phonebook application.
*/


#ifndef PHONEBOOK
#error "PHONEBOOK is undefined in config.h.  Are you using an old template?"
#endif

//! Initialize the phonebook.
void phonebook_init();
//! Draw the Phonebook
void phonebook_draw(int forced);
//! Exits the phonebook.
int phonebook_exit();

//! A button has been pressed for the phonebook.
int phonebook_keypress(char ch);

//! A fallthrough button in the third row for the phonebook.
int phonebook_fallthrough(char ch);
