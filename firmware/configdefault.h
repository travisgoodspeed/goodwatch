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

#ifndef DAPNETRIC
#define DAPNETRIC 0
#warning "DAPNET pager RIC is not defined in config.h"
#endif

#ifndef COREVOLTAGE
#define COREVOLTAGE 0
#endif

//Override this in config.h if you're testing an applet.
#ifndef DEFAULTAPP
#define DEFAULTAPP 0
#endif

/* This array holds the entries triggered by numbered buttons in the
   OOK applet.  The first two bytes of each entry at MDMCFG4 and
   MDMCFG3 to set the bitrate, followed by the packet to be
   transmitted at that rate.  See the GoodWatch wiki for details and
   sample projects to reverse engineer these settings.
 */

#ifndef OOKBUTTONS
#define OOKBUTTONS \
  /* Magicfly doorbell first, 200us.*/ \
  {.name="doorbell", .frequency=433.96e6, .mdmcfg4=0xf7, .mdmcfg3=0x93, \
  .tx_data="\x00\xe8\xe8\x88\x88\xee\x88\x8e\x88\xee\x8e\x88\xee\x80\x00\x00"}, \
  /* Settings for the Eco-Worthy Digital Motor Controller, ~430us */	\
  {.name="motor A", .frequency=433.96e6, .mdmcfg4=0x86, .mdmcfg3=0xd9, \
  .tx_data="\x00\xee\x8e\x8e\x8e\x8e\x8e\x8e\x8e\x88\x88\x88\xee\x80\x00\x00"}, \
  {.name="motor B", .frequency=433.96e6, .mdmcfg4=0x86, .mdmcfg3=0xd9, \
  .tx_data="\x00\xee\x8e\x8e\x8e\x8e\x8e\x8e\x8e\x88\x88\xee\x88\x80\x00\x00"}, \
  {.name="motor C", .frequency=433.96e6, .mdmcfg4=0x86, .mdmcfg3=0xd9, \
  .tx_data="\x00\xee\x8e\x8e\x8e\x8e\x8e\x8e\x8e\x88\xee\x88\x88\x80\x00\x00"}, \
  {.name="motor D", .frequency=433.96e6, .mdmcfg4=0x86, .mdmcfg3=0xd9, \
  .tx_data="\x00\xee\x8e\x8e\x8e\x8e\x8e\x8e\x8e\xee\x88\x88\x88\x80\x00\x00"}, \
    /* Settings for the Eco-Worthy Digital Motor Controller, should be
       ~430us but ~340 works. */					\
  {.name="relay A", .frequency=433.96e6, .mdmcfg4=0x86, .mdmcfg3=0xd9, \
  .tx_data="\x00\x00\xe8\xe8\xee\x88\xe8\x8e\xe8\x88\xee\xe8\x88\x8e\x80\x00"}, \
  {.name="relay B", .frequency=433.96e6, .mdmcfg4=0x86, .mdmcfg3=0xd9, \
  .tx_data="\x00\x00\xe8\xe8\xee\x88\xe8\x8e\xe8\x88\xee\xe8\x88\xe8\x80\x00"}, \
  {.name="relay C", .frequency=433.96e6, .mdmcfg4=0x86, .mdmcfg3=0xd9, \
  .tx_data="\x00\x00\xe8\xe8\xee\x88\xe8\x8e\xe8\x88\xee\xe8\x8e\x88\x80\x00"}, \
  {.name="relay D", .frequency=433.96e6, .mdmcfg4=0x86, .mdmcfg3=0xd9, \
  .tx_data="\x00\x00\xe8\xe8\xee\x88\xe8\x8e\xe8\x88\xee\xe8\xe8\x88\x80\x00"}
#endif
