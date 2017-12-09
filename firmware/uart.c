/*! \file uart.c
  \brief UART Driver for the GoodWatch
  
  
  The goal here is to use P1.5 and P1.6 as a UART, just as the BSL
  does.

  Because we *also* use these pins for the MODE and SET buttons, we
  need to be a bit tricky about their initialization, and the UART may
  not be used at the same time as those watch buttons.  The general
  idea is that uartactive will be set to 1 when the UART receives its
  first commands.
  
  Similarly, we can't handle the UART at our normal rate of 32kHz, so
  the first command must be sent as individual bytes to raise the CPU
  speed.

*/

#include<stdint.h>
#include<stdio.h>
#include<msp430.h>

#include "uart.h"
#include "monitor.h"

//! Set to 1 if the UART is active.
int uartactive=0;

//! Initializes the UART if the host is listening.
void uart_init(){
  printf("Initializing UART ");

  PMAPPWD = 0x02D52;
  P1MAP5 = PM_UCA0RXD; // Map UCA0RXD output to P1.5 
  P1MAP6 = PM_UCA0TXD; // Map UCA0TXD output to P1.6 
  PMAPPWD = 0;
  
  P1DIR |= BIT6;         // Set P1.6 as TX output
  P1SEL |= BIT5 + BIT6;  // Select P1.5 & P1.6 to UART function

  UCA0CTL1 |= UCSWRST;                 // **Put state machine in reset**
  UCA0CTL1 |= UCSSEL_1;                // ACLK
  UCA0BR0 = 3;                         // 32kHz 9600 (see User's Guide)
  UCA0BR1 = 0;                         // 32kHz 9600
  UCA0MCTL = UCBRS_3 + UCBRF_0;        // Modln UCBRSx=0, UCBRFx=0,

  UCA0CTL1 &= ~UCSWRST;                // **Initialize USCI state machine**
  UCA0IE |= UCRXIE;                    // Enable USCI_A0 RX interrupt
  //UCA0IE |= UCTXIE;                    // Enable USCI_A0 TX interrupt

}

//! Transmit a byte to the UART.
void uart_tx(uint8_t byte){
  /* Wait for the buffer to free up, then send the byte.  It'd be
     better to populate a ring buffer and dump then on TXIFG as
     appropriate.
   */
  while (!(UCA0IFG&UCTXIFG));
  UCA0TXBUF = byte;
}

#define UARTBUFLEN 32
uint8_t uart_buffer[UARTBUFLEN];



static uint16_t outlength, outindex, outcrc;

//! Handle sending a UART byte.
static void handle_txbyte(){
  /* For convenience, we use the same format and checksums as the BSL
     format.

     "\x00"+ll+lh+msg+crc
  */
  
  //State machine.
  static enum {IDLE,LL,LH,MSG,CRCL,CRCH} state=IDLE;

  //Do nothing whne the length is null.
  if(outlength==0)
    return;
  
  do{
    switch(state){
    case IDLE:
      /* Send 0x00 0x80 as the first bytes of the message. */
      uart_tx(0x00);
      uart_tx(0x80);
      state=LL;
      break;
    case LL:
      uart_tx(outlength&0xFF);
      state=LH;
      break;
    case LH:
      uart_tx((outlength>>8));
      outindex=0;
      state=MSG;
      break;
    case MSG:
      uart_tx(uart_buffer[outindex++]);
      if(outindex==outlength)
	state=CRCL;
      break;
    case CRCL:
      uart_tx(outcrc&0xFF);
      state=CRCH;
      break;
    case CRCH:
      uart_tx((outcrc>>8));
      state=IDLE;
      
      break;
    }
  }while(state!=IDLE);
}

//! Handle a UART byte.
static void handle_rxbyte(uint8_t byte){
  /* For convenience, we use the same format and checksums as the BSL
     format.

     "\x80"+ll+lh+msg+crc
  */
  
  //State machine.
  static enum {IDLE,LL,LH,MSG,CRCL,CRCH} state=IDLE;
  //Length and index.
  static uint16_t length, index, crc;
  

  switch(state){
  case IDLE:
    /* Wait for 0x80 as the first byte of the message. */
    if(byte==0x80)
      state=LL;
    break;
  case LL:
    length=byte;
    state=LH;
    break;
  case LH:
    length|= ((uint16_t)byte)<<8;
    index=0;
    if(length>UARTBUFLEN){
      printf("Buffer length error.\n");
      state=IDLE;
    }else{
      state=MSG;
    }
    break;
  case MSG:
    uart_buffer[index++]=byte;
    if(index==length)
      state=CRCL;
    break;
  case CRCL:
    crc=byte;
    state=CRCH;
    break;
  case CRCH:
    crc|= ((uint16_t)byte)<<8;
    state=IDLE;
    
    outlength=monitor_handle(uart_buffer, length);
    outindex=0;
    outcrc=crc;
    handle_txbyte();
    break;
  }
  
}


// Echo back RXed character, confirm TX buffer is ready first
void __attribute__ ((interrupt(USCI_A0_VECTOR))) USCI_A0_ISR (void){
  switch(UCA0IV&~1){
  case 0:break;                             // Vector 0 - no interrupt
  case 2:                                   // Vector 2 - RXIFG
    handle_rxbyte(UCA0RXBUF);
    break;
  case 4:
    //printf("TXIFG was triggered.\n");
    //handle_txbyte();
    break;                             // Vector 4 - TXIFG
  default: break;
  }
}

