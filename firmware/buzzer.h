/*! \file buzzer.h
  \brief Buzzer driver.

  This module provides the ability to configure and sound the buzzer attached to P2.7
 */

//! Buzzer init function
void buzzer_init();

//! Alarm ringing status indicator
static int alarm_ringing;

//! Sound the alarm
void alarm_trigger();

//! Interrupt vector routine
void buzzer_interrupt_routine();

//! Play a tone on the buzzer
void tone(int tone, int duration);

