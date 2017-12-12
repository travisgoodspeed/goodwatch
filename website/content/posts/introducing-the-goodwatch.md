+++
categories = []
date = "2017-12-12T16:01:17-05:00"
description = ""
draft = false
tags = []
thumbnail = "images/goodwatch.jpg"
title = "Introducing the GoodWatch"

+++


Howdy y'all,

A while back, I was daydreaming of a nifty wristwatch.  It would have
years of battery life, with an MSP430 microcontroller and RPN
calculator.  It would be able to receive and transmit on amateur radio
frequencies, in a variety of digital modes.  It would look spiffy.  So
I built the damned thing, and it's called the GoodWatch.

I began by disassembling the Casio 3208 watch module from a Casio
CA-53W calculator watch, and measuring its circuit board which you can
see on the right.  Pieces of sticky note over a pad would cause segments
to fail, allowing me to learn the pinout before making my own board,
shown on the left.

{{% img src="images/gw10-lcdpinout.jpg" w="600" %}}

I then assembled a watch with this board, writing firmware libraries
for the LCD, keypad, and power management.

{{% img src="images/gw10-assembly.jpg" w="600" %}}

The CC430F6137 that I chose can't quite control all of the pixels, but
with three commons and all available segment pins, I was able to get
everything except for the day-of-week pixels in the upper right corner.

{{% img src="images/alllcd.jpg" w="600" %}}

After that, it wasn't much work to toss together applications.  Each
application consists of a quick bit of C code, with handler functions
for entry, exit, and refreshing the screen.

{{% img src="images/gw10-lcd.jpg" w="600" %}}

In idle mode, the GoodWatch10 (shown on the left) can easily implement
all features of the original Casio.  It has ~5 years of battery life,
knows days of the week for the next two thousand years, and has a
handy RPN calculator.

{{% img src="images/bothclocks.jpg" w="600" %}}

It also has a hex editor, because no proper lady or gentleman would be
caught in public without one on the wrist.  Here the hex editor is
diagnosing a clock fault by reading the appropriate register.  (Now
there's a test case for that, of course.)

{{% img src="images/hexdiagnosis.jpg" w="600" %}}

"But Travis," you say, "What about a disassembler?  What if you're
stuck in an hour-long SCRUM meeting and need to reverse engineer your
watch's firmware with pen and paper to retain your own sanity?"

An MSP430 disassembler is built-in, of course.

{{% img src="images/disassembler.jpg" w="600" %}}


And while the GoodWatch10 was certainly the coolest hex editor watch
to wear last month, things can be niftier.  In this photo, it a
GoodWatch20 is beaconing my ham radio callsign to a Yaesu 817 as Morse
code.

{{% img src="images/cw.jpg" w="600" %}}

This tiny green wire--the only cosmetic blemish of the watch--uses the
stainless steel watchband as a random wire antenna.  It's not well
tuned, but it gets the job done.

{{% img src="images/antenna.jpg" w="600" %}}

The firmware also includes a serial debug monitor, so after flashing
firmware and a codeplug of radio frequencies, you can self-test the
device or send and receive packets from the air.

{{% img src="images/gw20-pins.jpg" w="600" %}}

The radio is based on the same CC1101 core that the GirlTech IMME
used, so all the old IMME hacks are portable.  My reflexive jammer for
P25, Mike Ossmann's iClicker emulator, and Samy's OpenSesame can all
be adapted to this platform.

{{% img src="images/realmen.jpg" w="600" %}}

You can't leave the receiver idly receiving in the background for
power budget reasons, but otherwise the radio is fully functional.
Receive and transmit, everything from CW to 4FSK.

{{% img src="images/gfskbeacon.jpg" w="600" %}}

The codeplug is compiled from a textfile and loaded into Info Flash,
where it won't be accidentally overwritten by firmware updates.
Writing a CHIRP plugin won't be much trouble.

{{% img src="images/codeplug.png" w="600" %}}

Firmware for the watch is written in clean C89, designed to compile
with GCC on Debian/Stable with no weird third-party packages or
commercial compilers.  The radio features gracefully disable on
watches without a 26MHz crystal.

{{% img src="images/firmware.png" w="600" %}}

Because it can be a pain to debug radio configurations in device
firmware, the UART monitor provides easy Python access to all the
radio functions.  You can mock up a working prototype of your radio
application in host-side python, then rather directly port to C.

{{% img src="images/usbradio.jpg" w="600" %}}

Full hardware CAD and source code are available on Github.  PCBs will
be available soon.

[https://github.com/travisgoodspeed/goodwatch/](https://github.com/travisgoodspeed/goodwatch/)


Your neighbor,

--Travis KK4VCZ
