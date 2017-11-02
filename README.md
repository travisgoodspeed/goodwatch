Howdy y'all,

This is a side project of mine to build a replacement circuit board
for the Casio 3208 module, used in the Casio CA-53W and CA-506
calculator watches.  It is not compatible with the 3228 module used in
the Databank watches, which have four external buttons instead of two.
As a side project, it has no warranty whatsoever and you shouldn't use
it for anything.

In the rare case that you find this project to be useful, you owe me a
pint of good, hoppy pale ale.  All license to use this project is
revoked if you try to pass off a pilsner instead.

This project is not yet public, but it will eventually be open source.
Please give me time to verify it first.

73 from Pizza Rat City,
--Travis

## Software Status

Our firmware is freely available in this repository, compiling with
the standard MSP430 compiler packages that ship with Debian.  It
consists of a Clock, a Stopwatch, an RPN Calculator, and a Hex Memory
Viewer with Disassembler, all written in C.  It compiles in Debian
with all of the MSP430 packages installed.

Additionally, we've written out own client for the CC430's BootStrap
Loader (BSL).  You might find it handy for other projects involving
that chip, or other MSP430F5 and MSP430F6 chips without USB.  You will
need to be a member of the `dialout` group to flash the firmware.

## Hardware Status

Our replacement board is based around the CC430F6137, which combines
an MSP430 microcontroller with an LCD controller and a sub-GHz radio.

The initial version, the GoodWatch10, lacks a filter chain for the
radio and is only useful for verifying the correctness of the
non-radio portion of the design.  The design is confirmed to be
functional, and Travis is wearing one for daily use.  Assembly of the
GoodWatch10 is a bit difficult, as components are 0201 wherever
possible and the central chip is QFN.

The GoodWatch20 gerber files have been finalized, and boards have been
ordered.  It adds support for sound and a 433MHz radio.

See `BOM.txt` in the hardware directories for a parts list, and the
wiki for an assembly guide.
