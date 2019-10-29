/*! \file descriptor.c
  \brief Reads MSP430 TLV Descriptors.
  
  So the GoodWatch supports both the CC430F6137 and the CC430F6147,
  which differ in minor ways.  Rather than hardcode the differences,
  we can peek at the device descriptors at runtime.
  
  See section 1.13 of the Family Guide for details on the full
  configuration structure.  For now, we only parse the version number,
  but calibration information might by handy later.

CC430F6137
  1a00: 0606
  1a02: d4a9
  1a04: 3761 <!-- CC430F6137
  1a06: 1212
  1a08: 0a08 <!-- First TLV entry, Die Record (0x08) of 0x0A bytes.
  1a0a: 0129
  1a0c: 4713
  1a0e: 001d
  1a10: 0020
  1a12: fef8
  1a14: 1011
  1a16: 7fff
  1a18: ffff
  1a1a: 0896
  1a1c: 0a1b
  1a1e: 0675
  1a20: 0794
  1a22: 0529
  1a24: 0611
  1a26: 0612

CC430F6147
  1a00: 0606
  1a02: f2f2
  1a04: 8135 <!-- CC430F6147
  1a06: 1010

*/

#include<stdint.h>
#include<stdio.h>
#include<msp430.h>

#include "descriptor.h"

//! Dumps the CPU descriptor.
void descriptor_dump(){
  /*  Raw dumping is wasteful, but sometimes handy.
  uint16_t *tlv = (uint16_t*) 0x1a00;
  while(tlv < (uint16_t*) TLV_END){
    printf("%04x: %04x\n",(uint16_t) tlv, (uint16_t) tlv[0]);
    tlv++;
  }
  printf("\n");
  */
  

  switch(DEVICEID){
  case DEVICEID6137:
    printf("CC430F6137 Recognized\n");
    break;
  case DEVICEID6147:
    printf("CC430F6147 Recognized\n");
    break;
  default:
    printf("Unknown ChipID %04x\n", DEVICEID);
  }
}
