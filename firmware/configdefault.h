/*! \file configdefault.h
  \brief Default configuration values, to be overloaded in config.h
  
  Each of these options can be over-written by defining them in
  config.h, which is intentionally not included by the distribution.
  See configtemplate.h as an example when constructing your
  configuration.
*/


#ifndef CALLSIGN
#define CALLSIGN "N0CALL"
#warning "CALLSIGN is not defined in config.h"
#endif


