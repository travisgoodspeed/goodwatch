/*! \file configdefault.h
  \brief Default configuration values, to be overloaded in config.h
  
  Each of these options can be over-written by defining them in
  config.h, which is intentionally not included by the distribution.
  See configtemplate.h as an example when constructing your
  configuration.
*/

#include "config.h"

#ifndef CALLSIGN
#define CALLSIGN "N0CALL"
#warning "CALLSIGN is not defined in config.h"
#endif

#ifndef COREVOLTAGE
#define COREVOLTAGE 0
#endif

//Override this in config.h if you're testing an applet.
#ifndef DEFAULTAPP
#define DEFAULTAPP 0
#endif
