/*! \file submenu.h
  \brief Submenu selection tool.
*/


//! Draw the submenu selection.
void submenu_draw();

//! Change the selected applet.
void submenu_keypress(char c);

//! Draw the submenu selected.
void submenu_drawselected();

//! On exit, set the submenu app.
int submenu_exit();
