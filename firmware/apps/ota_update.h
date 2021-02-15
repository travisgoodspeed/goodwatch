/*! \file beacon.h
  \brief Beacon Application
  
*/

//! Enter the Beacon application.
void ota_update_init();

//! Exit the Beacon application.
int ota_update_exit();

//! Draw the Beacon screen.
void ota_update_draw();

//! Handle an incoming packet.
void ota_update_packetrx(uint8_t *packet, int len);

//! Callback after transmission.
void ota_update_packettx();

//! Keypress handler for the beacon applet.
int ota_update_keypress(char ch);

