/*! \file codeplug.c
  \brief Codeplug for stored radio frequencies.
  
  The codeplug itself is stored in info flash at 0x1800, as a sequence
  of twelve-byte entries as defined in codeplug.h.
*/

#include "codeplug.h"
#include "radio.h"

//! Array of codeplug entries in info flash.
struct codeplugentry *codeplug = (struct codeplugentry*) 0x1800;

//! Index of entry in the codeplug.
static int codeplugi=0;

//! Next codeplug entry.
void codeplug_next(){
  if(codeplug[++codeplugi].flags==0xFF)
    codeplugi=0;
}

//! Previous codeplug entry.
void codeplug_prev(){
  /* If we run beneath the beginning of the codeplug, we need to walk
     all the way to the end and then back one.
   */
  if(--codeplugi<0){
    //Walk to the end.
    while(codeplug[++codeplugi].flags!=0xFF);
    //Jump back one.
    codeplug--;
  }
}

//! Return the name of the codeplug entry.  8 bytes, no null terminator!
const char *codeplug_name(){
  return codeplug[codeplugi].name;
}

//! Sets the codeplug frequency.
void codeplug_setfreq(){
  radio_setrawfreq(
		   codeplug[codeplugi].freq2,
		   codeplug[codeplugi].freq1,
		   codeplug[codeplugi].freq0);
}


