/*! \file monitor.c
  \brief Debugging commands, often received over UART.

  These commands are transacted over the UART, in much the same way
  that the BSL commands would be transacted.
  
*/

#include<msp430.h>
#include<stdint.h>
#include<stdio.h>
#include<string.h>

#include "api.h"
#include "rng.h"

enum {
  SETTURBOMODE = 0x00,
  PEEK         = 0x01,
  POKE         = 0x02,
  LCDSTRING    = 0x03,
  DMESG        = 0x04,
  RANDINT      = 0x05,

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

//! Local command to generate and send n random integers.
static void send_randint(uint16_t n){

  uint16_t i;
  uint16_t len = n*2;
  uint16_t rints[n]; // are var len arrays ok here?

  for(i = 0; i < n; i++){ //pre-generate enough ints
    rints[i] = true_rand();
  }
  //Start the frame
  uart_tx(0x00);
  uart_tx(0x80);
  //Length
  uart_tx(len&0xFF);
  uart_tx(len>>8);
  //Send integers byte by byte
  for(i=0;i<len;i++){
    uart_tx(((char*)rints)[i]);
  }
  // no checksum ?
  uart_tx(0x00);
  uart_tx(0x00);
}

//! Local pointer to packet.c's buffer.
static uint8_t *packetbuf=0;
//! Local length.
static int packetlen=0;

//! Handles packet arrival, like an application would.
void monitor_packetrx(uint8_t *packet, int len){
  /* This function is called by app_packetrx() in apps.c when a packet
     arrives and the uart is active.
   */
  if(len<UARTBUFLEN-1){
    packetbuf=packet;
    packetlen=len;
  }else{
    packetbuf=0;
    packetlen=0;
    printf("Packet too large to return.\n");
  }
}

//! Handles a monitor receive command.
static int handlerx(uint8_t *buffer, int len){
  switch(radio_getstate()){
  case 13:
  case 14:
  case 15:
    //Receive mode, so we are still waiting on a packet.
    return 1; //No packet yet.
  case 1:
    //IDLE mode.  Return a packet if we have one, and switch to RX mode.
    
    if(packetbuf && packetlen){
      //Return the waiting packet.
      memcpy(buffer,packetbuf,packetlen);
      len=packetlen;
    }else{
      len=1;
    }

    //Flush the local buffer pointers and try to receive another.
    packetbuf=0;
    packetlen=0;
    packet_rxon();
    
    return len;
  default:
    //Unknown state.  Return to RX.
    packet_rxon();
    break;
  }
  
  return 1; //No packet yet.
}

//! Handle a monitor command.
int monitor_handle(uint8_t *buffer, int len){
  uint16_t *buffer16=(uint16_t*) buffer;

  switch(buffer[0]){
  case SETTURBOMODE: //One byte parameter, on or off.
    if(buffer[1]){
      uartactive=1;
      lcd_zero();
      lcd_string("monitor ");
    }else{
      uartactive=0;
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

  case RANDINT:
    send_randint((uint16_t)buffer16[1]);
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
    radio_writepower(0x25);
    //codeplug_setfreq();
    break;
  case RADIORX:
    return handlerx(buffer,len);
    break;
  case RADIOTX:
    if(radio_getstate()==22){
      printf("TX Overflow.\n");
    }
    
    radio_strobe(RF_SIDLE);
    //Wait for it to settle.
    while(radio_getstate()!=1);
    //Then transmit.
    printf("Transmitting %d bytes.\n",len-1);
    packet_tx(buffer+1,len-1);
    break;
  default:
    printf("Returning unknown monitor command.\n");
  }
  
  //In the absense of something specific, we'll just return the
  //command that we were given.;
  return len;
}
