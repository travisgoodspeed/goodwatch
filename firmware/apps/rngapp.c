 /*! \file rngapp.c
  \brief Demo random number generator app
  
  This app will display random numbers on the display.
  
  Press 0 to generate a new random number
  Press 7 to diaply it in hexadecimal
  Press / to display it in decimal 
*/

#include <stdio.h>
#include <stdlib.h>
#include <msp430.h>
#include "api.h"
#include "rng.h"

enum rng_disp_format_enum {DECIMAL, HEX};
enum rng_disp_format_enum rng_disp_format; 
static unsigned int last_num = 0;

//! Enter the rng tool.
void rngapp_init(){
  rng_disp_format = DECIMAL;
}
//! Exit the rng tool.
int rngapp_exit(){
  last_num = 0; //leave only your footprints!
  return 0;
}

//! Handle a keypress in the rng application.
void rngapp_keypress(char ch){
  switch(ch){
  case '0':
    last_num = true_rand();
    break;
  case '7': 
    rng_disp_format = HEX;
    break;
  case '/': 
    rng_disp_format = DECIMAL;
    break;
  }
}

//! Draw the screen 
void rngapp_draw(){
  lcd_string("        ");
  if (!last_num) {
    lcd_string("press 0 ");
  } else {
    if (rng_disp_format == DECIMAL) {
        lcd_number(last_num);
      } else {
        lcd_hex(last_num);
      }
  }
}

