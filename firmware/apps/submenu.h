/*! \file submenu.h
  \brief Submenu selection tool.
*/


//! Draw the submenu selection.
void submenu_draw();

//! Change the selected applet.
int submenu_keypress(char c);

//! Lets a keypress fall through from the clock to the select submenu applet.
int submenu_fallthrough(char ch);

//! On exit, set the submenu app.
int submenu_exit();

//! Enter the submenu.
void submenu_init();
