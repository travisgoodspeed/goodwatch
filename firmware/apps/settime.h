/* \file settime.h
   \brief Applet for setting the time.  Not in the normal menu.
*/

//! Initialize the time setter.
void settime_init();
//! Exit to the next application.
int settime_exit();
//! Draw the setting time.
void settime_draw(int forced);

//! A button has been pressed for the clock.
int settime_keypress(char ch);
