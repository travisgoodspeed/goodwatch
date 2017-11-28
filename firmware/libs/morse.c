/*! \file morse.c
  \brief Morse code convenience functions.
*/

#include <msp430.h>
#include <stdio.h>
#include "api.h"


//! Send a message in Morse, just dits and dahs.
static void radio_morse_raw(const char *msg){
  if(has_radio)
    while(*msg!='\0'){
      
      //Transmit if not a space.
      if(*msg!=' ')
	radio_strobe(RF_STX);
      
      if(*msg=='.' || *msg=='*')
	__delay_cycles(2000);
      else if(*msg=='-')
	__delay_cycles(6000);
      else if(*msg==' ')
	__delay_cycles(6000);
      
      //End transmission.
      radio_strobe(RF_SIDLE);
      //Intercharacter space.
      __delay_cycles(1000);
      
      msg++;
    }
}

//! Sends an individual letter in morse.
static void radio_morse_char(char c){
  //Only work with upper case letters.
  if(c>='a' && c<='z')
    c&=~0x20;
  
  switch(c){
  case 'A':
    radio_morse_raw(".- "); break;
  case 'B':
    radio_morse_raw("-..."); break;
  case 'C':
    radio_morse_raw("-.-."); break;
  case 'D':
    radio_morse_raw("-.."); break;
  case 'E':
    radio_morse_raw("."); break;
  case 'F':
    radio_morse_raw("..-."); break;
  case 'G':
    radio_morse_raw("--."); break;
  case 'H':
    radio_morse_raw("...."); break;
  case 'I':
    radio_morse_raw(".."); break;
  case 'J':
    radio_morse_raw(".---"); break;
  case 'K':
    radio_morse_raw("-.-"); break;
  case 'L':
    radio_morse_raw(".-.."); break;
  case 'M':
    radio_morse_raw("--"); break;
  case 'N':
    radio_morse_raw("-."); break;
  case 'O':
    radio_morse_raw("---"); break;
  case 'P':
    radio_morse_raw(".--."); break;
  case 'Q':
    radio_morse_raw("--.-"); break;
  case 'R':
    radio_morse_raw(".-."); break;
  case 'S':
    radio_morse_raw("..."); break;
  case 'T':
    radio_morse_raw("-"); break;
  case 'U':
    radio_morse_raw("..-"); break;
  case 'V':
    radio_morse_raw("...-"); break;
  case 'W':
    radio_morse_raw(".--"); break;
  case 'X':
    radio_morse_raw("-..-"); break;
  case 'Y':
    radio_morse_raw("-.--"); break;
  case 'Z':
    radio_morse_raw("--.."); break;
    
  case '1':
    radio_morse_raw(".----"); break;
  case '2':
    radio_morse_raw("..---"); break;
  case '3':
    radio_morse_raw("...--"); break;
  case '4':
    radio_morse_raw("....-"); break;
  case '5':
    radio_morse_raw("....."); break;
  case '6':
    radio_morse_raw("-...."); break;
  case '7':
    radio_morse_raw("--..."); break;
  case '8':
    radio_morse_raw("---.."); break;
  case '9':
    radio_morse_raw("----."); break;
  case '0':
    radio_morse_raw("-----"); break;
  default:
    printf("Unknown Morse character: %c\n",c);
    break;
  }
  radio_morse_raw(" ");
}


//! Send a message in Morse.
void radio_morse(const char *msg){
  if(has_radio)
    while(*msg!='\0'){
      switch(*msg){
      case '.':
      case '*':
	radio_morse_raw(".");
	break;
      case '-':
	radio_morse_raw("-");
	break;
      case ' ':
	radio_morse_raw(" ");
	break;
      default:
	radio_morse_char(*msg);
	break;
      }
      msg++;
    }
}
