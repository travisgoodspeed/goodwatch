/*! \file sidebutton.h
  \brief Sidebutton driver.
*/

//! Initialize the IO pins.
void sidebutton_init();
//! Is the mode button pressed?
int sidebutton_mode();
//! Is the Program/Set button pressed?
int sidebutton_set();
