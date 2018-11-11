/*! \file dmesg.h
  \brief Dmesg viewer application.
*/

//! Enter the dmesg editor.
void dmesgapp_init();
//! Exit the dmesg editor.
int dmesgapp_exit();
//! Draw the screen.
void dmesgapp_draw(int forced);

//! A button has been pressed for the dmesg editor.
int dmesgapp_keypress(char ch);
