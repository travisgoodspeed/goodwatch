/*! \file tuner.h
  \brief Tuning application and RSSI tool.
*/

//! Enter the radio tool.
void tuner_init();
//! Exit the radio tool.
int tuner_exit();
//! Draw the screen and increase the count.
void tuner_draw();

//! Tuner keypress callback.
void tuner_keypress(char ch);
