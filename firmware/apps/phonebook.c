/*! \file phonebook.h
  \brief Phonebook application.
*/

#include "api.h"

//! Pointer to the current phonebook entry.
static const char* pbptr;


//! Initialize the phonebook.
void phonebook_init(){
  pbptr=phonebook;
}

//! Draw the Phonebook
void phonebook_draw(int forced){
  //Draw the word.
  lcd_string(pbptr);
}


//! Exits the phonebook.
int phonebook_exit(){
  //Return 0 so we can move back to the clock.
  return 0;
}


//! A button has been pressed for the phonebook.
int phonebook_keypress(char ch){
  switch(ch){
    //Third (fallthrough) row shows one word each of the current row.
  case '1':
    pbptr=pb_firstword(pbptr);
    break;
  case '2':
    pbptr=pb_firstword(pbptr);
    pbptr=pb_nextword(pbptr);
    break;
  case '3':
    pbptr=pb_firstword(pbptr);
    pbptr=pb_nextword(pbptr);
    pbptr=pb_nextword(pbptr);
    break;
  case '-':
    pbptr=pb_firstword(pbptr);
    pbptr=pb_nextword(pbptr);
    pbptr=pb_nextword(pbptr);
    pbptr=pb_nextword(pbptr);
    break;

    
    //Bottom row moves the line or entry.
  case '0':
    pbptr=pb_nextword(pbptr);
    break;
  case '+':
    pbptr=pb_nextline(pbptr);
    break;
  default:
    return 0; //no need to redraw
  }
  return 1; //redraw
}



//! A fallthrough button in the third row for the phonebook.
int phonebook_fallthrough(char ch){
  phonebook_draw(phonebook_keypress(ch));
  return 0;
}
