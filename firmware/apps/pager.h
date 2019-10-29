/*! \file pager.h
  \brief POCSAG Pager Application
*/

//! Enter the Pager application.
void pager_init();

//! Exit the Pager application.
int pager_exit();

//! Draw the Pager screen.
void pager_draw();

//! Handle an incoming packet.
void pager_packetrx(uint8_t *packet, int len);

//! Keypress handler for the pager applet.
int pager_keypress(char ch);

