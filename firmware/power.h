/*! \file power.h
  \brief Power management library for the CC430F6137.
*/

//! Returns non-zero if in anything but the lowest power mode.
int power_ishigh();

//! Sets the core core voltage.
int power_setvcore (int level);
