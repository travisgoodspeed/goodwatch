/*! \file bcd.h
  \brief Functions for converting decimal to BCD.
*/

//! ROM table for conversion to BCD.
extern const char bcdtable[];

//! Fetch from a ROM table for conversion to BCD.
#define int2bcd(i) bcdtable[i]
