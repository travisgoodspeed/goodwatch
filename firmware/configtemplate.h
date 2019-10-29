
// You definitely want to uncomment and change this to your callsign.
//#define CALLSIGN "N0CALL"
// And maybe also your DAPNET RIC.
//#define DAPNETRIC 123456


// The core voltage should be 0 for a coin cell, 2 or 3 for quality power.
//#define COREVOLTAGE 3

// Uncomment this to emulate the SET button by holding + and - at once.
//#define EMULATESET

//Uncomment this to set the default RTC Calibration.  Positive is slow, negative is fast.
#define RTCCALIBRATE -28


/* This array holds the entries triggered by numbered buttons in the
   OOK applet.  The first two bytes of each entry at MDMCFG4 and
   MDMCFG3 to set the bitrate, followed by the packet to be
   transmitted at that rate.  See the GoodWatch wiki for details and
   sample projects to reverse engineer these settings.
 */

#define OOKBUTTONS		\
  /* Magicfly doorbell first, 200us.*/ \
  "\xf7\x93" "\x00\xe8\xe8\x88\x88\xee\x88\x8e\x88\xee\x8e\x88\xee\x80\x00\x00", \
    /* Settings for the Eco-Worthy Digital Motor Controller, should be
       ~430us but ~340 works. */					\
    "\x86\xd9" "\x00\xee\x8e\x8e\x8e\x8e\x8e\x8e\x8e\x88\x88\x88\xee\x80\x00\x00", \
    "\x86\xd9" "\x00\xee\x8e\x8e\x8e\x8e\x8e\x8e\x8e\x88\x88\xee\x88\x80\x00\x00", \
    "\x86\xd9" "\x00\xee\x8e\x8e\x8e\x8e\x8e\x8e\x8e\x88\xee\x88\x88\x80\x00\x00", \
    "\x86\xd9" "\x00\xee\x8e\x8e\x8e\x8e\x8e\x8e\x8e\xee\x88\x88\x88\x80\x00\x00", \
    /* No-name wireless relay controller. ~340us */			\
    "\x86\xd9" "\x00\x00\xe8\xe8\xee\x88\xe8\x8e\xe8\x88\xee\xe8\x88\x8e\x80\x00", \
    "\x86\xd9" "\x00\x00\xe8\xe8\xee\x88\xe8\x8e\xe8\x88\xee\xe8\x88\xe8\x80\x00", \
    "\x86\xd9" "\x00\x00\xe8\xe8\xee\x88\xe8\x8e\xe8\x88\xee\xe8\x8e\x88\x80\x00", \
    "\x86\xd9" "\x00\x00\xe8\xe8\xee\x88\xe8\x8e\xe8\x88\xee\xe8\xe8\x88\x80\x00"

