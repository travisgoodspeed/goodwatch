 /*! \file phrase.c
  \brief Demo passphrase generator app
  
  This app will display random words on the desplay. Six of more of these words 
  can be used as a passphrase. Inspired by diceware.
  
  7,4,1 or 0 cycle to next word.
*/

#include<stdio.h>
#include<stdlib.h>
#include<msp430.h>
#include "api.h"
#include "rng.h"
#include "words.h"

static const char *rand_word(){
  unsigned int r = ((unsigned int)true_rand()) % NUMWORDS;
  return word_array[r];
}

static const char *last_string = NULL;

//! Enter the passphrase tool.
void phrase_init(){
  last_string = "press 0 "; //clean up last displayed word
}
//! Exit the passphrase tool.
int phrase_exit(){
  last_string = "press 0 "; //leave only your footprints!
  return 0;
}

//! Handle a keypress in the passphrase application.
int phrase_keypress(char ch){
  switch(ch){
  case '7': 
  case '4':
  case '1':
  case '0': 
    last_string = rand_word();
    break;
  }
  return 1;
}

//! Draw the screen 
void phrase_draw(){
  lcd_string("        ");
  lcd_string(last_string);
}

