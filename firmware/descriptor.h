/*! \file descriptor.h
  \brief Reads MSP430 TLV Descriptors.
*/

//! Dumps the CPU descriptor.
void descriptor_dump();


//! Model number of the chip.  Bytes probably swapped for endian reasons.
#define DEVICEID (*((uint16_t*)0x1a04))
/*
  3761 for CC430F6137
  8135 for CC430F6147
 */

#define DEVICEID6137 0x3761
#define DEVICEID6147 0x8135
