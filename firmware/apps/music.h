/*! \file music.h
  \brief music Application
  
*/

//! Enter the music application.
void music_init();

//! Exit the music application.
int music_exit();

//! Draw the music screen.
void music_draw();

//! Handle an incoming packet.
void music_packetrx(uint8_t *packet, int len);

//! Keypress handler for the music applet.
int music_keypress(char ch);

//! Fallthrough handler for the music applet.
int music_fallthrough(char ch);
