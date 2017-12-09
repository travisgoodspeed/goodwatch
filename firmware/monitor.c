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
  LCDSTRING    = 0x03,
  DMESG        = 0x04,

  RADIOONOFF   = 0x10,
  RADIOCONFIG  = 0x11,
  RADIORX      = 0x12,
  RADIOTX      = 0x13
} monitor_verb;


//! Local command to send the dmesg buffer.
static void send_dmesg(){
  uint16_t i;
  char *dmesg_buffer=(char*)0x2400;
  
  //Start the frame.
  uart_tx(0x00);
  uart_tx(0x80);
  //Length
  uart_tx(DMESGLEN&0xFF);
  uart_tx(DMESGLEN>>8);
  
  //dmesg
  for(i=0;i<DMESGLEN;i++){
    uart_tx(dmesg_buffer[i]);
  }
  //checksum
  uart_tx(0x00);
  uart_tx(0x00);
}

//! Handle a monitor command.
int monitor_handle(uint8_t *buffer, int len){
  uint16_t *buffer16=(uint16_t*) buffer;

  switch(buffer[0]){
  case SETTURBOMODE: //One byte parameter, on or off.
    if(buffer[1]){
      uartactive=1;
      lcd_zero();
      lcd_string("nnonitor");
      ucs_fast();
    }else{
      uartactive=0;
      ucs_slow();
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

  case LCDSTRING:
    lcd_zero();
    lcd_string((char*) buffer+1);
    break;
    
  case DMESG:
    send_dmesg();
    len=0;
    break;


  case RADIOONOFF: //One byte parameter, on or off.
    if(buffer[1]){
      radio_on();
    }else{
      radio_off();
    }
    break;
  case RADIOCONFIG:  //Byte pairs come next.  Host must null-terminate.
    radio_writesettings(buffer+1);
    break;
  case RADIORX:
    //Not sure how to tie this in.
    break;
  case RADIOTX:
    packet_tx(buffer+1,len-1);
    break;
  }
  
  //In the absense of something specific, we'll just return the
  //command that we were given.
  return len;
}
