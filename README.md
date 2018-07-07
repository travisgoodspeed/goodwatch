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

In addition to the source code, there is handy documentation in the
[wiki](https://github.com/travisgoodspeed/goodwatch/wiki) and a
general interest website at [goodwatch.org](https://goodwatch.org/).

73 from Knoxville,

--Travis

## Software Status

Our firmware is freely available in this repository, compiling with
the standard MSP430 compiler packages that ship with Debian.  It
consists of a Clock, a Stopwatch, an RPN Calculator, and a Hex Memory
Viewer with Disassembler, all written in C.  It compiles in Debian
with all of the MSP430 packages installed.

On watches with a radio, we have Morse and GFSK transmitters, as well
as an OOK transmitter that will command cheap remote-controlled
relays.  The radio is accessible from a host computer over the UART
for building base stations and repeaters, or for rapidly prototyping
radio applications in Python.  P25 and DMR support might come soon.

Additionally, we've written our own client for the CC430's BootStrap
Loader (BSL).  You might find it handy for other projects involving
that chip, or other MSP430F5 and MSP430F6 chips without USB.  You will
need to be a member of the `dialout` group to flash the firmware.

[Firmware documentation](http://goodwatch.org/doxygen/) can be built
with Doxygen.

## Hardware Status

Our replacement board is based around the CC430F6137, which combines
an MSP430 microcontroller with an LCD controller and a sub-GHz radio.
Thanks to pin compatibility, our fancy linker scripts produce a
firmware image that works just as well on the more modern CC430F6147
chip.

The initial version, the GoodWatch10, lacks a radio but supports all
other features, such as the RPN calculator, hex editor, and
disassembler.  Assembly of the GoodWatch10 is no longer recommended,
as the PCB requires a bit of whittling to fit the case.

The GoodWatch21 is now our primary target, featuring minor corrections
to layout and a functional radio.  The radio is in active use, and the
filter chain is verified to work.

The GoodWatch30 will be manufactured in early 2018 with a wider filter
and other radio modifications.

See `BOM.txt` in the hardware directories for a parts list, and the
wiki for an assembly guide.


## Related Projects

[Pluto](https://github.com/carrotIndustries/pluto) is a replacement
PCB for the Casio F-91W that is also based on an MSP430.  We are
jealous of that project's simulator and energy monitor utility.

[FaradayRF](https://faradayrf.com/) is a CC430 radio board intended
for amateur use, with a 500mW amplifier and GPS receiver.

[openchronos-ng-elf](https://github.com/BenjaminSoelberg/openchronos-ng-elf)
seems to be the most recently maintained firmware for TI's OpenChronos
devkit, which uses the CC430.
