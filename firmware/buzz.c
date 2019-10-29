/*! \file buzz.c
  \brief Handy buzzer functions.
  
  These are currently untested, as the buzzer causes the GoodWatch20
  to glitch out.  We hope to have proper buzzer support in the
  GoodWatch21.
*/

#include<msp430.h>

#include<stdio.h>

//! Make a quick buzz.
void buzz(unsigned int count){
  //Start the timer.
  if(count){
    //Output select mode for P2.7.
    P2DIR|=0x80;
    P2SEL|=0x80;
    
    //P2.7 might need high drive strength for the piezo.
    //P2DS|=0x80;

    
    TA1CCR0 = count;
    TA1CTL |= MC__UP;
  }else{
    //Stop the timer when it's not in use.
    TA1CTL = TACLR | TASSEL__SMCLK | MC__STOP;

    //Input mode for the pin, so we don't accidentally leak power.
    P2DIR&=~0x80;
    P2SEL&=~0x80;
  }
}

//! blocking tone generation function with duration
void tone(unsigned int freq, unsigned int duration) {
  // output select mode for P2.7.
  P2DIR|=0x80;
  P2SEL|=0x80;
  // set timer for desired frequency
  TA1CCR0 = 32768 / freq;
  TA1CTL |= MC__UP;
  // delay until we want to stop the tone
  int i;
  for (i = 0; i < duration; i++) {
    __delay_cycles(2900);
  }
  // stop the tone and reset the pin
  TA1CTL = TACLR | TASSEL__SMCLK | MC__STOP;
  P2DIR&=~0x80;
  P2SEL&=~0x80;
}


//! Initializes the buzzer port.
void buzz_init(){
  TA1CTL=0;
  
  //Unlock port mapping.
  PMAPKEYID=0x2d52;
  if(PMAPCTL&1)
    printf("Ugh, port mapping is locked.\n");
  
  //Route timera1 to the pin.  This might conflict with rng.c.
  P2MAP7=PM_TA1CCR0A;;
  
  //Lock port mapping.
  PMAPKEYID=0x96a5;
  
  
  TA1CTL = TACLR | TASSEL__SMCLK | MC__STOP;
  TA1CCTL0 = OUTMOD_4;
  TA1CCTL0 &= ~CCIE;

  P2SEL|=0x80;
}

