/*! \file ook.h
  \brief OOK Application
*/

//! Enter the OOK application.
void ook_init();

//! Exit the OOK application.
int ook_exit();

//! Draw the OOK screen.
void ook_draw();

//! Handle an incoming packet.
void ook_packetrx(uint8_t *packet, int len);
//! Callback after transmission.
void ook_packettx();

//! Keypress handler for the OOK applet.
int ook_keypress(char ch);

