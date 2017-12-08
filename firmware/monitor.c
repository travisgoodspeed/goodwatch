/*! \file monitor.c
  \brief Debugging commands, often received over UART.

  These commands are transacted over the UART, in much the same way
  that the BSL commands would be transacted.
  
*/

#include<msp430.h>
#include<stdint.h>
#include<stdio.h>

#include "api.h"

enum {
  SETTURBOMODE = 0x00,
  PEEK         = 0x01,
  POKE         = 0x02,
  LCD          = 0x03,
  DMESG        = 0x04
} monitor_verb;


//! Handle a monitor command.
int monitor_handle(uint8_t *buffer, int len){
  uint16_t *buffer16=(uint16_t*) buffer;

  switch(buffer[0]){
  case SETTURBOMODE: //One byte parameter, on or off.
    if(buffer[1]){
      uartactive=1;
      lcd_zero();
      lcd_string("nnonitor");
      UCSCTL4 = SELM_3 + SELS_0 + SELA_0;  //XT1 for ACLK and SMCLK, MCLK from DCO.
    }else{
      uartactive=0;
      UCSCTL4 = SELM_0 + SELS_0 + SELA_0;  //XT1 for everything; very slow CPU.
    }
    break;
    
  case PEEK: //Null, then 16-bit parameter for address.
    if(buffer[1]==0){
      buffer16[1] = *((uint16_t*)buffer16[1]);
    }
    break;
    
  case POKE:
    if(buffer[1]==0){ //Null, the address, then value.
      *((uint16_t*)buffer16[1]) = *((uint16_t*)buffer16[2]);
    }
    break;

  case DMESG:
    
    break;
  }
  
  //In the absense of something specific, we'll just return the
  //command that we were given.
  return len;
}
