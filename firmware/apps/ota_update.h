/*! \file ota_update.h
  \brief OTA Update Application
  
*/

//! Enter the OTA Update application.
void ota_update_init();

//! Exit the OTA Update application.
int ota_update_exit();

//! Draw the OTA Update screen.
void ota_update_draw();

//! Handle an incoming packet.
void ota_update_packetrx(uint8_t *packet, int len);

//! Callback after transmission.
void ota_update_packettx();

//! Keypress handler for the OTA Update applet.
int ota_update_keypress(char ch);

