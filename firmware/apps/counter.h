/*! \file counter.h
  \brief Frequency Counter Application
*/

//! Enter the Counter application.
void counter_init();

//! Exit the Counter application.
int counter_exit();

//! Draw the Counter screen.
void counter_draw();

//! Handle an incoming packet.
void counter_packetrx(uint8_t *packet, int len);
//! Callback after transmission.
void counter_packettx();

//! Keypress handler for the Counter applet.
int counter_keypress(char ch);

