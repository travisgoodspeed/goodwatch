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

//Settings for a DC relay, 341us/bit OOK.
#ifndef OOKMDMCFG4
#define OOKMDMCFG4 0x86
#endif
#ifndef OOKMDMCFG3
#define OOKMDMCFG3 0xd9
#endif
#ifndef OOKBUTTONA
#define OOKBUTTONA "\x00\x00\xe8\xe8\xee\x88\xe8\x8e\xe8\x88\xee\xe8\x88\x8e\x80\x00"
#endif
#ifndef OOKBUTTONB
#define OOKBUTTONB "\x00\x00\xe8\xe8\xee\x88\xe8\x8e\xe8\x88\xee\xe8\x88\xe8\x80\x00"
#endif
#ifndef OOKBUTTONC
#define OOKBUTTONC "\x00\x00\xe8\xe8\xee\x88\xe8\x8e\xe8\x88\xee\xe8\x8e\x88\x80\x00"
#endif
#ifndef OOKBUTTOND
#define OOKBUTTOND "\x00\x00\xe8\xe8\xee\x88\xe8\x8e\xe8\x88\xee\xe8\xe8\x88\x80\x00"
#endif
