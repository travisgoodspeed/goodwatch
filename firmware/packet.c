/*! \file packet.c
  \brief Packet handling library.
  
  This library is a companion to radio.c, allowing for reception and
  transmission of packets.
  
  For now, we are limited to sixty byte packets that fit within the
  radio's internal FIFO buffer.
  
*/

#include<msp430.h>
#include<stdio.h>
#include "api.h"

//! Receive packet buffer, with room for RSSI and LQI.
uint8_t rxbuffer[PACKETLEN+2];
//! Length of received packet.
uint8_t rxlen;

//! Transmit packet buffer.
uint8_t txbuffer[PACKETLEN];

static int transmitting, receiving;

//! Initialize the packet variables.  Only called from radio_on().
void packet_init(){
  transmitting=0;
  receiving=0;
}

//! Switch to receiving packets.
void packet_rxon(){
  receiving=1;
  
  RF1AIES |= BIT9;    // Falling edge of RFIFG9
  RF1AIFG &= ~BIT9;   // Clear a pending interrupt
  RF1AIE  |= BIT9;    // Enable the interrupt
  
  radio_strobe( RF_SRX );
}

//! Stop receiving packets.
void packet_rxoff(){
  RF1AIE &= ~BIT9;    // Disable RX interrupts
  RF1AIFG &= ~BIT9;   // Clear pending IFG

  /* If RXOFF is called in the middle of a packet, it's necessary to
     flux the RX queue.
   */
  radio_strobe( RF_SIDLE );
  radio_strobe( RF_SFRX  );
  receiving=0;
}

//! Transmit a packet.
void packet_tx(uint8_t *buffer, uint8_t length){
  if(transmitting){
    printf("Refusing to transmit with pending packet.\n");
    return;
  }
  transmitting=1;
  
  RF1AIES |= BIT9;                          
  RF1AIFG &= ~BIT9;                         // Clear pending interrupts
  RF1AIE |= BIT9;                           // Enable TX end-of-packet interrupt

  
  //Write the packet into the buffer.
  radio_writeburstreg(RF_TXFIFOWR, buffer, length);     

  //Strobe into transmit mode.
  radio_strobe( RF_STX );
  
}


//! Interrupt handler for incoming packets.
void __attribute__ ((interrupt(CC1101_VECTOR)))
packet_isr (void) {
  int rf1aiv=RF1AIV;
  int state;
  
  switch(rf1aiv&~1){       // Prioritizing Radio Core Interrupt 
    case  0: break;                         // No RF core interrupt pending
    case  2: break;                         // RFIFG0
    case  4: break;                         // RFIFG1
    case  6: break;                         // RFIFG2
    case  8: break;                         // RFIFG3
    case 10: break;                         // RFIFG4
    case 12: break;                         // RFIFG5
    case 14: break;                         // RFIFG6
    case 16: break;                         // RFIFG7
    case 18: break;                         // RFIFG8
    case 20:                                // RFIFG9
      if(receiving){//End of RX packet.
	
	//Wait for end of packet.
	do{
	  state=radio_getstate();
	  //__delay_cycles(8500);
	}while(state==13 || state==14 || state==15 || state==20 || state==21);


	if(state==1){
	  // Read the length byte from the FIFO.
	  rxlen = radio_readreg( RXBYTES );
	  //__delay_cycles(8500);
	  
	  /* We read no more than our buffer. */
	  radio_readburstreg(RF_RXFIFORD, rxbuffer,
			     rxlen>PACKETLEN?PACKETLEN:rxlen);
	  
	  //Inform the application.
	  app_packetrx(rxbuffer,rxlen);
	}else if(state==17){
	  printf("RX Overflow.  Idling.\n");
	  radio_strobe(RF_SIDLE);
	}else{
	  printf("Unknown RX state %d.\n",state);
	  printf("rx, tx = %d, %d\n",
		 receiving,
		 transmitting);
	}
        /*
        // Check the CRC results
        if(RxBuffer[CRC_LQI_IDX] & CRC_OK){
          printf("Correct CRC\n");
	}else{
	  printf("Incorrect CRC.\n");
	}
	*/
      }else if(transmitting){ //End of TX packet.
	//printf("Transmitted packet.\n");
        RF1AIE &= ~BIT9;     // Disable TX end-of-packet interrupt
        transmitting = 0;
	//Inform the application.
	app_packettx();
      }else{
	printf("Unexpected packet ISR.\n");
      }
      break;
    case 22: break;                         // RFIFG10
    case 24: break;                         // RFIFG11
    case 26: break;                         // RFIFG12
    case 28: break;                         // RFIFG13
    case 30: break;                         // RFIFG14
    case 32: break;                         // RFIFG15
  }
  
  //printf("Handled.\n");
  //__bic_SR_register_on_exit(LPM3_bits);
}
