/*! \file stopwatch.h
  \brief Stopwatch application.
 */

//! A button has been pressed for the stopwatch.
int stopwatch_keypress(char ch);

//! Enter the stopwatch.
void stopwatch_init();
//! Exit the stopwatch.
int stopwatch_exit();
//! Draw the screen and increase the count.
void stopwatch_draw();
