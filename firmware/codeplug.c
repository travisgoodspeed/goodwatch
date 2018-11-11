/*! \file codeplug.c
  \brief Codeplug for stored radio frequencies.
  
  The codeplug itself is stored in info flash at 0x1800, as a sequence
  of twelve-byte entries as defined in codeplug.h.
*/

#include<stdio.h>
#include<string.h>

#include "codeplug.h"
#include "radio.h"

extern const char codeplugstr[];

//! Array of codeplug entries in info flash.
struct codeplugentry *codeplug =
  (struct codeplugentry*) codeplugstr; //formerly 0x1800

static struct codeplugentry *selectedentry;

//! Special codeplug entry for the VFO.
struct codeplugentry vfoentry={
  .flags=0x00,
  .name="vfo mode"
};

//! Index of entry in the codeplug.
static int codeplugi=0;

//! Initialize the codeplug at boot.
void codeplug_init(){
  do{
    codeplug_next();
  }while(codeplugi);

  selectedentry = &codeplug[codeplugi];
}

//! Next codeplug entry.
void codeplug_next(){
  if(codeplug[++codeplugi].flags==0xFF)
    codeplugi=0;
  
  selectedentry = &codeplug[codeplugi];
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
    codeplugi--;
  }

  selectedentry = &codeplug[codeplugi];
}

//! Return the name of the codeplug entry.  8 bytes, no null terminator!
const char *codeplug_name(){
  static char name[9];

  if(selectedentry->flags!=0xFF){
    memcpy(name,selectedentry->name,8);
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
  if(selectedentry->flags!=0xFF){
    /* We have a codeplug, so set the current entry's frequency. */
    radio_setrawfreq(selectedentry->freq2,
		     selectedentry->freq1,
		     selectedentry->freq0);
  }else{
    /* We don't have a codeplug, so default to 434.0 */
    radio_setfreq(434000000);
  }
}

//! Sets the VFO frequency.
void codeplug_setvfofreq(float freq){
  float freqMult = (0x10000 / 1000000.0) / 26;
  uint32_t num = freq * freqMult;

  //Store the frequency into the VFO entry.
  vfoentry.freq2 = (num >> 16) & 0xFF;
  vfoentry.freq1 = (num >> 8) & 0xFF;
  vfoentry.freq0 = num & 0xFF;
  
  //Select the VFO entry.
  selectedentry = &vfoentry;

  //Tune the radio.
  codeplug_setfreq();
}


//! Gets the codeplug frequency.
uint32_t codeplug_getfreq(){
  static uint32_t oldhex=0, oldnum=0;
  uint32_t hex=
    0xFF0000l & (((uint32_t) selectedentry->freq2)<<16);
  hex|= (0xFF00l & (selectedentry->freq1<<8));
  hex|= (0xFFl & (selectedentry->freq0));
  
  if(selectedentry->flags!=0xFF){
    /* We have a codeplug, so set the current entry's frequency. */
    
    //Return the old value if it hasn't changed.
    if(oldhex==hex)
      return oldnum;
    
    //Otherwise calculate the new value and return it.
    oldhex=hex;
    oldnum=hex*396.728515625;
    return oldnum;
  }else{
    /* We don't have a codeplug, so default to 434.0 */
    return 434000000;
  }
}

