/*! \file countdown.h
  \brief Countdown application.
 */

//! A button has been pressed for the countdown.
int countdown_keypress(char ch);

//! Enter the countdown.
void countdown_init();
//! Exit the countdown.
int countdown_exit();
//! Draw the screen and increase the count.
void countdown_draw(int forced);
