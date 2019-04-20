/*! \file pocsag.h
  \brief Efficient microcontroller implementation of POCSAG reception.
*/


#define MAXPAGELEN 32

//! ID of the most recently received POCSAG message.
extern uint32_t pocsag_lastid;
//! String of the last page length.
extern char pocsag_buffer[MAXPAGELEN];


//! Handle one codeword in POCSAG.
void pocsag_handleword(uint32_t word);

//! Call this once for every new POCSAG batch.
void pocsag_newbatch();


