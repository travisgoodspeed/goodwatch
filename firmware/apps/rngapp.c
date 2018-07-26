 /*! \file rngapp.c
  \brief Demo random number generator app
  
  This app will display random numbers.
  
  Press 0 to generate a new random number.
  Press 7 to display it in hexadecimal.
  Press / to display it in decimal.
*/

#include <stdio.h>
#include <stdlib.h>
#include <msp430.h>
#include "api.h"
#include "rng.h"

enum rng_disp_format_enum {DECIMAL, HEX};
enum rng_disp_format_enum rng_disp_format; 
static unsigned int num = 0, last_num = 0;

//! Enter the rng tool.
void rngapp_init(){
  rng_disp_format = DECIMAL;
}
//! Exit the rng tool.
int rngapp_exit(){
  num = last_num = 0; //leave only your footprints!
  return 0;
}

//! Handle a keypress in the rng application.
int rngapp_keypress(char ch){
  switch(ch){
  case '0':
    num = true_rand();
    break;
  case '7': 
    rng_disp_format = HEX;
    last_num=0; //Dump old number to reconvert.
    break;
  case '/': 
    rng_disp_format = DECIMAL;
    last_num=0; //Dump old number to reconvert.
    break;
  }
  return 1; //Redraw.
}

//! Draw the screen, only if the number has changed.
void rngapp_draw(){
  if (!num) {
    lcd_string("press 0 ");
  } else if (num!=last_num) {
    last_num=num;
    lcd_string("        ");
    if (rng_disp_format == DECIMAL) {
      lcd_number(num);
    } else {
      lcd_hex(num);
    }
  }
}

