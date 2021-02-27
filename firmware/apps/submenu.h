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


//! Fall through to the submenu for the third row.
int submenu_fallthrough(char ch);

//! Fall through to the submenu app for a transmission.
void submenu_packettx();
