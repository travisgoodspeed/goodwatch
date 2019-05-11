/*! \file power.c
  \brief Power management library for the CC430F6137.
  
  These library functions are responsible for powering up the CC430
  when necessary for the radio, and for dropping power at all other
  times.
*/

#include<stdio.h>
#include<msp430.h>

//! Returns non-zero if in anything but the lowest power mode.
int power_ishigh(){
  return
    ( PMMCTL0_L & PMMHPMRE_L)  //High power mode.
    || (PMMCTL0 & PMMCOREV_3) //VCore in anything but 0 mode.
    ;
}

//! Step the core voltage up
static unsigned int power_vcoreup (unsigned char level) {
  unsigned int PMMRIE_backup,SVSMHCTL_backup;

  // Open PMM registers for write access
  PMMCTL0_H = 0xA5;

  // Disable dedicated Interrupts to prevent that needed flags will be
  // cleared
  PMMRIE_backup = PMMRIE;
  PMMRIE &= ~(SVSMHDLYIE | SVSMLDLYIE | SVMLVLRIE | SVMHVLRIE | SVMHVLRPE);
  // Set SVM highside to new level and check if a VCore increase is
  // possible
  SVSMHCTL_backup = SVSMHCTL;
  PMMIFG &= ~(SVMHIFG | SVSMHDLYIFG);
  SVSMHCTL = SVMHE | SVMHFP | (SVSMHRRL0 * level);
  // Wait until SVM highside is settled
  while ((PMMIFG & SVSMHDLYIFG) == 0);
  // Disable full-performance mode to save energy
  SVSMHCTL &= ~SVSLFP ;
  
  // Check if a VCore increase is possible
  if ((PMMIFG & SVMHIFG) == SVMHIFG){
    // Vcc is too low for a Vcore increase, so recover the previous
    // settings.

    printf("VCC too low to raise core.\n");
    
    PMMIFG &= ~SVSMHDLYIFG;
    SVSMHCTL = SVSMHCTL_backup;
    // Wait until SVM highside is settled
    while ((PMMIFG & SVSMHDLYIFG) == 0);
    // Clear all Flags
    PMMIFG &= ~(SVMHVLRIFG | SVMHIFG | SVSMHDLYIFG
		| SVMLVLRIFG | SVMLIFG | SVSMLDLYIFG);
    // backup PMM-Interrupt-Register
    PMMRIE = PMMRIE_backup;
      
    // Lock PMM registers for write access
    PMMCTL0_H = 0x00;
    return 0;//Not set.
  }
  
  // Set also SVS highside to new level
  //-> Vcc is high enough for a Vcore increase
  SVSMHCTL |= SVSHE | (SVSHRVL0 * level);
  // Set SVM low side to new level
  SVSMLCTL = SVMLE | SVMLFP | (SVSMLRRL0 * level);
  // Wait until SVM low side is settled
  while ((PMMIFG & SVSMLDLYIFG) == 0);
  // Clear already set flags
  PMMIFG &= ~(SVMLVLRIFG | SVMLIFG);
  // Set VCore to new level
  PMMCTL0_L = PMMCOREV0 * level;
  // Wait until new level reached
  if (PMMIFG & SVMLIFG)
    while ((PMMIFG & SVMLVLRIFG) == 0);
  // Set also SVS/SVM low side to new level
  PMMIFG &= ~SVSMLDLYIFG;
  SVSMLCTL |= SVSLE | (SVSLRVL0 * level);
  // wait for lowside delay flags
  while ((PMMIFG & SVSMLDLYIFG) == 0);

  // Disable SVS/SVM Low
  // Disable full-performance mode to save energy
  SVSMLCTL &= ~(SVSLE+SVMLE+SVSLFP );

  // Clear all Flags
  PMMIFG &= ~(SVMHVLRIFG | SVMHIFG | SVSMHDLYIFG
	      | SVMLVLRIFG | SVMLIFG | SVSMLDLYIFG);
  // backup PMM-Interrupt-Register
  PMMRIE = PMMRIE_backup;

  // Lock PMM registers for write access
  PMMCTL0_H = 0x00;
  return 1;//okay
}

//! Step the voltage core down.
static unsigned int power_vcoredown (unsigned char level) {
  unsigned int PMMRIE_backup;

  // Open PMM registers for write access
  PMMCTL0_H = 0xA5;

  // Disable dedicated Interrupts to prevent that needed flags will be cleared
  PMMRIE_backup = PMMRIE;
  PMMRIE &= ~(SVSMHDLYIE | SVSMLDLYIE | SVMLVLRIE | SVMHVLRIE | SVMHVLRPE);

  // Set SVM high side and SVM low side to new level
  PMMIFG &= ~(SVMHIFG | SVSMHDLYIFG | SVMLIFG | SVSMLDLYIFG);
  SVSMHCTL = SVMHE | SVMHFP | (SVSMHRRL0 * level);
  SVSMLCTL = SVMLE | SVMLFP | (SVSMLRRL0 * level);
  // Wait until levels are settled.
  while ((PMMIFG & SVSMHDLYIFG) == 0 || (PMMIFG & SVSMLDLYIFG) == 0);


  // Set VCore to new level
  PMMCTL0_L = PMMCOREV0 * level;
  
  /*
    printf("Changing high and low sides.\n");
    // Set also SVS highside and SVS low side to new level
    PMMIFG &= ~(SVSHIFG | SVSMHDLYIFG | SVSLIFG | SVSMLDLYIFG);
    
    printf("a\n");
    SVSMHCTL |= SVSHE | SVSHFP | (SVSHRVL0 * level);
    printf("b\n");
    SVSMLCTL |= SVSLE | SVSLFP | (SVSLRVL0 * level);
    */


  // Wait until SVS high side and SVS low side is settled
  //while ((PMMIFG & SVSMHDLYIFG) == 0 || (PMMIFG & SVSMLDLYIFG) == 0);
    
  // Disable full-performance mode to save energy
  SVSMHCTL &= ~SVSLFP;
  // Disable SVS/SVM Low
  // Disable full-performance mode to save energy
  SVSMLCTL &= ~(SVSLE+SVMLE+SVSLFP);


  // Clear all Flags
  PMMIFG &= ~(SVMHVLRIFG | SVMHIFG | SVSMHDLYIFG
	      | SVMLVLRIFG | SVMLIFG | SVSMLDLYIFG);
  // backup PMM-Interrupt-Register
  PMMRIE = PMMRIE_backup;
  // Lock PMM registers for write access
  PMMCTL0_H = 0x00;

  if ((PMMIFG & SVMHIFG) == SVMHIFG)
    // Highside is still too low for the adjusted VCore Level
    return 0;
  else
    return 1;
}


//! Sets the core core voltage.
int power_setvcore (int level) {
  int oldlevel, origlevel;
  int status = 1;

  //Mask off the level.
  level &= PMMCOREV_3;
  //Current core voltage.
  origlevel = oldlevel = (PMMCTL0 & PMMCOREV_3);
  
  // step by step increase or decrease
  while (((level != oldlevel) && (status)) || (level < oldlevel)) {
    if (level > oldlevel)
      status = power_vcoreup(++oldlevel);
    else
      status = power_vcoredown(--oldlevel);
    __delay_cycles(850);
  }

  if(origlevel!=(PMMCTL0 & PMMCOREV_3))
    printf("Power %d->%d.\n",
           origlevel,
           PMMCTL0 & PMMCOREV_3);
  return status;
}
