Howdy y'all,

This is a side project of mine to build a replacement circuit board
for the Casio 3208 module, used in the Casio CA-53W and related
calculator watches.  As a side project, it has no warranty whatsoever
and you shouldn't use it for anything.

In the rare case that you find this project to be useful, you owe me a
pint of good, hoppy pale ale.  All license to use this project is
revoked if you try to pass off a pilsner instead.

This project is not yet public, but it will eventually be open source.
Please give me time to verify it first.

73 from Pizza Rat City,
--Travis

## Hardware Status

This replacement board is based around the CC430F6137, which combines
an MSP430 microcontroller with an LCD controller and a sub-GHz radio.

The initial version, the GoodWatch10, lacks a filter chain for the
radio and is only useful for verifying the correctness of the
non-radio portion of the design.  It is believed to be correct, but
has not yet been manufactured or assembled for verification.

Assembly is a bit difficult, as components are 0201 wherever possible
and the central chip is QFN.

## Software Status

The firmware has not yet been written.  We will aim for a
GCC-compatible image, focusing first on battery life and watch
functionality, only later on fancy radio features.  Because of limited
power availability, initial applications will be transmit-only.

