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

#include<stdint.h>
#include<msp430.h>

#include "power.h"
#include "radio.h"


//! Set to 1 at boot if the watch has a radio.
int has_radio;

//! Called at boot.  Gracefully fails if no radio.
void radio_init(){
  /* If the radio components are missing, the AVCC_RF lines will be
     unconnected and the radio will immediately have a low voltage error.
  */
  if(RF1AIFERR & 1){
    has_radio=0;
    return;
  }

  //With no voltage error, we have a radio!  Let's power cycle it to
  //make sure that won't cause problems later.
  has_radio=1;
  radio_on();
  radio_off();
}

//! Turns the radio on.  Returns zero on failure.
int radio_on(){
  if(!has_radio){
    return 0; //Failure.
  }

  //Enable high power mode so that LPM3 can be used with an active
  //radio.
  PMMCTL0_H = 0xA5;
  PMMCTL0_L |= PMMHPMRE_L;
  PMMCTL0_H = 0x00;

  //Step up the core voltage a bit.
  power_setvcore(2);
  __delay_cycles(850);
  power_setvcore(3);
  __delay_cycles(850);

  //Strobe the radio to reset it.
  radio_strobe(RF_SRES);
  radio_strobe(RF_SNOP);
  
  return 1; //Success
}

//! Turns the radio off.
int radio_off(){

  //Disable high-power mode.
  PMMCTL0_H = 0xA5;
  PMMCTL0_L &= ~PMMHPMRE_L;
  PMMCTL0_H = 0x00;

  return 1;
}


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

//! Write to a register in the radio.
void radio_writereg(uint8_t addr, uint8_t value){
  // Wait until the radio is ready.
  while (!(RF1AIFCTL1 & RFINSTRIFG));
  
  // Send the address and instruction.
  RF1AINSTRB = (addr | RF_SNGLREGWR);
  // And the value.
  RF1ADINB = value;
}

//! Strobe a rdaio register.
uint8_t radio_strobe(uint8_t strobe){
  uint8_t  statusByte = 0;
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
          while ((RF1AIN&0x04)== 0x04);     // chip-ready ?
          // Delay for ~810usec at 1.05MHz CPU clock, see erratum RF1A7
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
