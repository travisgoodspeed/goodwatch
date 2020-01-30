/*! \file shaders.h
  \implements the somfy RTS protocol
*/


// remote commands
#define SHADERS_UP 0x2
#define SHADERS_DOWN 0x4
#define SHADERS_MY 0x1
#define SHADERS_PROG 0x8

// id to identify the remote. Adapt to your needs.
#define SHADERS_BASE_ID 0x232320

// TODO double check that
#define SHADERS_PACKET_LENGTH 16

// https://pushstack.wordpress.com/somfy-rts-protocol/
#define SHADERS_RAW_PAYLOAD_LENGTH 7

#define SHADERS_ENC_KEY 0xA8

//! Enter the shaders application.
void shaders_init();

//! Exit the shaders application.
int shaders_exit();

//! Draw the shaders screen.
void shaders_draw();

//! Handle an incoming packet.
void shaders_packetrx(uint8_t *packet, int len);
//! Callback after transmission.
void shaders_packettx();

//! Keypress handler for the shaders applet.
int shaders_keypress(char ch);

//    byte
//     0       1        2       3       4       5       6
// |-------|--------|-------|-------|-------|-------|-------|
// |  key  |ctrl|cks|  Rolling Code |   Address(A0|A1|A3)   |
// |-------|--------|-------|-------|-------|-------|-------|