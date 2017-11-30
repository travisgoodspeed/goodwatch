/*! \file codeplug.h
  \brief Codeplug for stored radio frequencies.
*/

#include <stdint.h>

#define CODEPLUGLEN 512

//! Single entry in the codeplug database.
struct codeplugentry {
  /* The flags byte is undefined; keep at zero for now. */
  const uint8_t flags;
  
  /* The frequency is stored in the CC1101-based radio's internal
     format.  See radio.c for example calculations.
   */
  const uint8_t freq2;
  const uint8_t freq1;
  const uint8_t freq0;

  /* The eight character name is *NOT* null terminated. */
  const char name[8];
};

//! Next codeplug entry.
void codeplug_next();
//! Previous codeplug entry.
void codeplug_prev();

//! Return the name of the codeplug entry.  8 bytes, no null terminator!
const char *codeplug_name();
//! Sets the codeplug frequency.
void codeplug_setfreq();
