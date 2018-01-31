/*! \file beacon.h
  \brief Beacon Application
  
*/

//! Enter the Beacon application.
void beacon_init();

//! Exit the Beacon application.
int beacon_exit();

//! Draw the Beacon screen.
void beacon_draw();

//! Handle an incoming packet.
void beacon_packetrx(uint8_t *packet, int len);

//! Keypress handler for the beacon applet.
void beacon_keypress(char ch);

