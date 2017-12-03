/*! \file codeplug.c
  \brief Codeplug for stored radio frequencies.
  
  The codeplug itself is stored in info flash at 0x1800, as a sequence
  of twelve-byte entries as defined in codeplug.h.
*/

#include<stdio.h>
#include<string.h>

#include "codeplug.h"
#include "radio.h"

//! Array of codeplug entries in info flash.
struct codeplugentry *codeplug = (struct codeplugentry*) 0x1800;

//! Index of entry in the codeplug.
static int codeplugi=0;

//! Initialize the codeplug at boot.
void codeplug_init(){
  do{
    printf("Codeplug entry '%s': %02x 0x%02x%02x%02x\n",
	   codeplug_name(),
	   codeplug[codeplugi].flags,
	   codeplug[codeplugi].freq2,
	   codeplug[codeplugi].freq1,
	   codeplug[codeplugi].freq0
	   );
    codeplug_next();
  }while(codeplugi);
  
}

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
  static char name[9];

  if(codeplug[codeplugi].flags!=0xFF){
    memcpy(name,codeplug[codeplugi].name,8);
    name[8]=0;
  }else{
    /* We don't seem to have a codeplug, so we write "Missing" to the
       display as a warning.  When we support a VFO mode, we'll switch
       to it here.
     */
    strcpy(name,"MISSING");
  }
  return name;
}

//! Sets the codeplug frequency.
void codeplug_setfreq(){
  if(codeplug[codeplugi].flags!=0xFF){
    /* We have a codeplug, so set the current entry's frequency. */
    radio_setrawfreq(codeplug[codeplugi].freq2,
		     codeplug[codeplugi].freq1,
		     codeplug[codeplugi].freq0);
  }else{
    /* We don't have a codeplug, so default to 434.0 */
    radio_setfreq(434000000);
  }
}

