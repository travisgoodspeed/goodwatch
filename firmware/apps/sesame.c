/*! \file ook.c
  \brief Open Sesame App

  
  Howdy y'all,
  
  Half a decade ago, Samy Kamkar demoed the Open Sesame application
  for the GirlTech IMME, which used a de Bruijn sequence to brute
  force 8 to 12-bit garage door openers in eight seconds.
  
  This is a port of his fine tool the GoodWatch platform.
  
  --Travis
  
*/

#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <msp430.h>
#include "api.h"

void sesame_packettx();


#define LEN 16 //Bytes
static char lastch = 0;



void sesame_init() {
  if (!has_radio) {
    app_next();
  }
  lcd_string("sesame");
}


int sesame_exit() {
  radio_off();
  return 0;
}

/*================================= U I =================================*/


// Keypress Handler
int sesame_keypress(char ch) {
  return 0;
}

// Button Mapping

void sesame_packettx() {

}

// Draw Screen
void sesame_draw() {
  int state = radio_getstate();
  
  switch (state) {
  case 0: //Skip
  case 1:
    break;
  case 19: //RX IDLE between transmit packets.
    break;
  case 22: // TX OVERFLOW
    printf("TX Overflow.\n");
    radio_strobe(RF_SIDLE);
    break;
  default:
    lcd_hex(state);
    break;
  }
}
