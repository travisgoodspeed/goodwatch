/*! \file radio.c
  \brief RF1A Radio Module Driver
  
  This is our neighborly little driver for the RF1A radio module.
  While we try really hard to avoid lasagna-code by over-abstracting
  drivers, the radio needs to be abstracted for good reasons, with
  exceptions to that rule in applications as appropriate.
  
  In general:
  
  1) Not all watches have a radio, so your application ought to
  gracefully fail if XT2 is not present.

  2) The CR2016 battery lasts forever when telling the time, but
  you'll quickly run out of juice if you leave the radio on.  For this
  reason, your app should probably time out after three minutes and
  you should never expect to be receiving in the background.

  3) Remember that transmitting is cheaper than receiving, because a
  transmitter can shut down afterward.
  
  4) The CPU runs at 32kHz by default.  You can speed it up, but at
  the cost of power consumption.
  
*/

#include<stdio.h>
#include<stdint.h>
#include<msp430.h>

#include "radio.h"

//! Cleared to zero at the first radio failure.
//! Read a register from the radio.
uint8_t radio_readreg(uint8_t addr){
  // Check for valid configuration register address, 0x3E refers to PATABLE 
  if ((addr <= 0x2E) || (addr == 0x3E))
    // Send address + Instruction + 1 dummy byte (auto-read)
    RF1AINSTR1B = (addr | RF_SNGLREGRD);    
  else
    // Send address + Instruction + 1 dummy byte (auto-read)
    RF1AINSTR1B = (addr | RF_STATREGRD);    
  
  while (!(RF1AIFCTL1 & RFDOUTIFG) );
  
  //Reading the data clears the interrupt flag.
  return RF1ADOUTB;
}

//! Read multiple bytes from a register.
void radio_readburstreg(uint8_t addr,
			uint8_t *buffer, uint8_t count){
  unsigned int i;

  if(count > 0){
    while (!(RF1AIFCTL1 & RFINSTRIFG));       // Wait for INSTRIFG
    RF1AINSTR1B = (addr | RF_REGRD);          // Send addr of first conf. reg. to be read 
                                              // ... and the burst-register read instruction
    for (i = 0; i < (count-1); i++) {
      while (!(RFDOUTIFG&RF1AIFCTL1));        // Wait for the Radio Core to update the RF1ADOUTB reg
      buffer[i] = RF1ADOUT1B;                 // Read DOUT from Radio Core + clears RFDOUTIFG
                                              // Also initiates auo-read for next DOUT byte
    }
    buffer[count-1] = RF1ADOUT0B;             // Store the last DOUT from Radio Core  
  }
}

//! Write multiple bytes to a register.
void radio_writeburstreg(uint8_t addr,
			 uint8_t *buffer, uint8_t count){
  unsigned char i;
  
  if(count > 0){
    while (!(RF1AIFCTL1 & RFINSTRIFG));       // Wait for the Radio to be ready for next instruction
    RF1AINSTRW = ((addr | RF_REGWR)<<8 ) + buffer[0]; // Send address + Instruction
    
    for (i = 1; i < count; i++) {
      RF1ADINB = buffer[i];                   // Send data
      while (!(RFDINIFG & RF1AIFCTL1));       // Wait for TX to finish
    }
    i = RF1ADOUTB;                            // Reset RFDOUTIFG flag which contains status byte
  }
}

//! Write to a register in the radio.
void radio_writereg(uint8_t addr, uint8_t value){
  // Wait until the radio is ready.
  while (!(RF1AIFCTL1 & RFINSTRIFG));
  
  // Send the address and instruction.
  RF1AINSTRB = (addr | RF_SNGLREGWR);
  // And the value.
  RF1ADINB = value;
}


//! Strobe a radio register.
uint8_t radio_strobe(uint8_t strobe){
	uint8_t  statusByte = 0;
	uint16_t count=0;
	uint16_t gdo_state;


	// Check for valid strobe command 
	if((strobe == 0xBD) || ((strobe >= RF_SRES) && (strobe <= RF_SNOP))){
		// Clear the Status read flag 
		RF1AIFCTL1 &= ~(RFSTATIFG);    

		// Wait for radio to be ready for next instruction
		while( !(RF1AIFCTL1 & RFINSTRIFG));


		// Write the strobe instruction
		if ((strobe > RF_SRES) && (strobe < RF_SNOP)){
			gdo_state = radio_readreg(IOCFG2);    // buffer IOCFG2 state
			radio_writereg(IOCFG2, 0x29);         // chip-ready to GDO2

			RF1AINSTRB = strobe; 
			if ( (RF1AIN&0x04)== 0x04 ) {         // chip at sleep mode
				if ( (strobe == RF_SXOFF) || (strobe == RF_SPWD) || (strobe == RF_SWOR) ) {

				}else{
					/* We'll get stuck in an infinite loop here if the radio
					crystal isn't available.
					*/
					while ((RF1AIN&0x04)== 0x04){
						if(count++>1000){
							//printf("Timeout in radio_strobe.  Broken XT2?\n");
							return 0xFF;
						}
					}

					// Delay for ~810usec at 1.05MHz CPU clock, see the CC430 Family User's
					// Guide,§ 25.3.3.7.1 page 698 in rev E. The delay is to provide time
					// for the radio's oscillator to stabilize.
					__delay_cycles(850);
				}
			}

			radio_writereg(IOCFG2, gdo_state);    // restore IOCFG2 setting

			while( !(RF1AIFCTL1 & RFSTATIFG) );
		}else{ //Active mode
			RF1AINSTRB = strobe;
		}

		statusByte = RF1ASTATB;
	}

	return statusByte;
}


//! Read the radio MARC state.
int radio_getstate(){
	int state;
	state=radio_readreg(MARCSTATE);
	return state;
}
