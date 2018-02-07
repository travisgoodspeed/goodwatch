/*! \file buzzer.h
  \brief Buzzer driver.

  This module provides the ability to configure and sound the buzzer attached to P2.7
 */


#include <msp430.h>
#include "api.h"

static int buzzer_active = 0;
static int buzzer_control_limit=0;
static int buzzer_control_count=0;

//! Alarm ringing status indicator
static int alarm_ringing=0;


void tone(int tone, int duration) {
    buzzer_active = 1;
    TA1CCTL0 = CCIE;
    TA0CTL = TASSEL_2 + MC__UP;
    TA0CCR0 = 32768 / tone;
    buzzer_control_limit = ((32768 / duration) / 1000) / TA0CCR0;
}

void buzzer_interrupt_routine(){
    if (buzzer_active){
        P2OUT ^= BIT7;
        if (buzzer_control_count++ > buzzer_control_limit) {
            buzzer_active = 0;
            buzzer_control_limit = 0;
            buzzer_control_count = 0;
            TA0CCR0 = 0x00;
            P2OUT &= ~BIT7;
        }
    }
}

//! Sound the alarm
void alarm_trigger() {
  alarm_ringing=1;
  tone(500,1000);
  __delay_cycles(32768);
  tone(500,1000);
  alarm_ringing=0;
}

void buzzer_init() {
    // Set 2.7 Out and Low
    P2DIR=BIT7;
    P2OUT &= ~BIT7;
}
