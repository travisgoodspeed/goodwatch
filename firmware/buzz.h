/*! \file buzz.h
  \brief Handy buzzer functions.
*/

//! Initializes the buzzer port.
void buzz_init();

//! Make a quick buzz.
void buzz(int);

//! blocking tone generation function with approximate duration in milliseconds
void tone(unsigned int freq, unsigned int duration);
