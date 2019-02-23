/*! \file clock.h
  \brief Clock application, the main one.
*/

//! Initialize the clock.
void clock_init();
//! Exit to the next application.
int clock_exit();
//! Draw the clock.
void clock_draw(int forced);

//! A button has been pressed for the clock.
int clock_keypress(char ch);

//! Plays the time as audio.
void clock_playtime(int hold);

//! Draw the time.
void draw_time(int redraw);
//! Draw the date.
void draw_date();
