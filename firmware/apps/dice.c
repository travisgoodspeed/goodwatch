 /*! \file dice.c
  \brief Demo passphrase generator app
  
  This app will display random words on the desplay. With bigger dictionary,
  these can be used as a passphrase. Inspired by diceware.
  
  7,4,1 or 0 cycle to next word.
*/

#include<stdio.h>
#include<stdlib.h>
#include<msp430.h>
#include "api.h"
#include "rng.h"
#include "dicewords.h"

static const char *rand_word(){
  unsigned int r = ((unsigned int)true_rand()) % NUMDICEWORDS;
  return word_array[r];
}

static const char *last_string = NULL;

//! Enter the dice tool.
void dice_init(){
  last_string = "press 0 "; //clean up last displayed word
}
//! Exit the dice tool.
int dice_exit(){
  last_string = "press 0 "; //leave only your footprints!
  return 0;
}

//! Handle a keypress in the dice application.
void dice_keypress(char ch){
  switch(ch){
  case '7': 
  case '4':
  case '1':
  case '0': 
    last_string = rand_word();
    break;
  }
}

//! Draw the screen 
void dice_draw(){
  lcd_string("        ");
  lcd_string(last_string);
}

