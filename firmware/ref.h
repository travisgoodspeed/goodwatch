/*! \file ref.h
  \brief Power reference module for the CC430F6137 and CC430F6147.
*/

//! Turn the reference on.
void ref_on();

//! Turn the reference off.
void ref_off();

//! Activate the module and turn it on.
void ref_init();

//! Switch the the static mode, which is power hungry but more reliable.
void ref_staticmode();

//! Switch to sample mode, which is more efficient.
void ref_sampledmode();

//! Print the REF status.
void ref_status();
