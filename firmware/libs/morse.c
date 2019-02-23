/*! \file morse.c
  \brief Morse code convenience functions.
*/

#include <msp430.h>
#include <stdio.h>
#include "api.h"



#define DITLEN 2000
#define DAHLEN 6000
#define SPALEN 6000
//Inter-character space.
#define ICSLEN 1000

//! Delay for a while.
static void morsedelay(uint16_t i){
  while(i--){
    __delay_cycles(100);
  }
}


//! Send a message in Morse, just dits and dahs.
static void radio_morse_raw(const char *msg){
  if(has_radio)
    while(*msg!='\0'){
      
      //Transmit if not a space.
      if(*msg!=' ')
	radio_strobe(RF_STX);
      
      if(*msg=='.' || *msg=='*')
	morsedelay(DITLEN);
      else if(*msg=='-')
	morsedelay(DAHLEN);
      else if(*msg==' ')
	morsedelay(SPALEN);
      
      //End transmission.
      radio_strobe(RF_SIDLE);
      //Intercharacter space.
      morsedelay(ICSLEN);
      
      msg++;
    }
}
//! Plays a message in Morse, just dits and dahs.
static void audio_morse_raw(const char *msg, const int held){
  while(*msg!='\0' && (!held || key_pressed())){
    //beep if not a space.
    if(*msg!=' ')
      buzz(32768 / 7000);
    
    if(*msg=='.' || *msg=='*')
      morsedelay(DITLEN);
    else if(*msg=='-')
      morsedelay(DAHLEN);
    else if(*msg==' ')
      morsedelay(SPALEN);
    
    //End transmission.
    buzz(0);
    //Intercharacter space.
    morsedelay(ICSLEN);
    
    msg++;
  }
}

//! Returns the string matching a morse character.
static char* morse_char(char c){
  //Only work with upper case letters.
  if(c>='a' && c<='z')
    c&=~0x20;

  /* This has no breaks in the switch because all cases return. */
  switch(c){
  case 'A':
    return(".- ");
  case 'B':
    return("-...");
  case 'C':
    return("-.-.");
  case 'D':
    return("-..");
  case 'E':
    return(".");
  case 'F':
    return("..-.");
  case 'G':
    return("--.");
  case 'H':
    return("....");
  case 'I':
    return("..");
  case 'J':
    return(".---");
  case 'K':
    return("-.-");
  case 'L':
    return(".-..");
  case 'M':
    return("--");
  case 'N':
    return("-.");
  case 'O':
    return("---");
  case 'P':
    return(".--.");
  case 'Q':
    return("--.-");
  case 'R':
    return(".-.");
  case 'S':
    return("...");
  case 'T':
    return("-");
  case 'U':
    return("..-");
  case 'V':
    return("...-");
  case 'W':
    return(".--");
  case 'X':
    return("-..-");
  case 'Y':
    return("-.--");
  case 'Z':
    return("--..");
    
  case '1':
    return(".----");
  case '2':
    return("..---");
  case '3':
    return("...--");
  case '4':
    return("....-");
  case '5':
    return(".....");
  case '6':
    return("-....");
  case '7':
    return("--...");
  case '8':
    return("---..");
  case '9':
    return("----.");
  case '0':
    return("-----");
  default:
    printf("Unknown Morse character: %c\n",c);
  }
  return "";
}


//! Sends an individual letter in morse.
static void radio_morse_char(char c){
  radio_morse_raw(morse_char(c));
  radio_morse_raw(" ");
}

//! plays an individual letter in morse.
static void audio_morse_char(char c, int held){
  audio_morse_raw(morse_char(c), held);
  audio_morse_raw(" ", held);
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



//! Send a message in Morse.
void audio_morse(const char *msg, const int held){
  /* If the held switch is set, then we only play so long as the
     button is held down. */

  while(*msg!='\0'){
    switch(*msg){
    case '.':
    case '*':
      audio_morse_raw(".", held);
      break;
    case '-':
      audio_morse_raw("-", held);
      break;
    case ' ':
      audio_morse_raw(" ", held);
      break;
    default:
      audio_morse_char(*msg, held);
      break;
    }
    msg++;
  }
}


