/*! \file packet.c
  \brief Packet handling library.
  
  This library is a companion to radio.c, allowing for reception and
  transmission of packets.
  
  For now, we are limited to sixty byte packets that fit within the
  radio's internal FIFO buffer.
  
*/

#include<msp430.h>
#include<stdio.h>

#include "packet.h"
#include "radio.h"
#include "debug.h"

//! Receive packet buffer, with room for RSSI and LQI.
// Needs to be aligned to word size, otherwise write to flash fails silently
static uint8_t rxbuffer[PACKETLEN+2] __attribute__ ((aligned (2)));
//! Length of received packet.
uint8_t rxlen;

static int transmitting=0, receiving=0;

extern void packet_rx(uint8_t *buf, int len);
extern void packet_tx_callback();

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
    //printf("Refusing to transmit with pending packet.\n");
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
__interrupt void packet_isr (void) {

	int rf1aiv=RF1AIV;
	int state;

	switch(rf1aiv&~1){       // Prioritizing Radio Core Interrupt 
		case 20:                                // RFIFG9
			if(receiving){   //End of RX packet.

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
					radio_readburstreg(RF_RXFIFORD, rxbuffer, rxlen > PACKETLEN ? PACKETLEN : rxlen);

					//Inform the application.
					packet_rx(rxbuffer, rxlen);

				}else if(state==17){
					L("RXO");
					radio_strobe(RF_SFRX);
					radio_strobe(RF_SRX);
				}else{
				}
			}else if(transmitting){ //End of TX packet.
				RF1AIE &= ~BIT9;     // Disable TX end-of-packet interrupt
				transmitting = 0;
				//Inform the updater
				packet_tx_callback();
			}else{
				//printf("Unexpected packet ISR.\n");
			}
			break;
	}

}
