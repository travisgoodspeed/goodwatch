/*! \file submenu.h
  \brief Submenu selection tool.
*/


//! Draw the submenu selection.
void submenu_draw();

//! Change the selected applet.
int submenu_keypress(char c);

//! On exit, set the submenu app.
int submenu_exit();

//! Enter the submenu.
void submenu_init();
