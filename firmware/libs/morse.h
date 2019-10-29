/*! \file morse.h
  \brief Morse convenience functions.
*/


//! Send a message in Morse.
void radio_morse(const char *msg);

//! Play a message in Morse.
void audio_morse(const char *msg, const int held);
