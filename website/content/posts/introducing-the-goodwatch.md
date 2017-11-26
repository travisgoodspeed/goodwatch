+++
categories = []
date = "2017-10-21T16:01:17-05:00"
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

After that, it wasn't much work to toss together applications for the
clock, calendar, timer, RPN calculator, and hex editor.  Each
application consists of a quick bit of C code, with handler functions
for entry, exit, and refreshing the screen.

{{% img src="images/gw10-lcd.jpg" w="600" %}}

Having functional wristwatches with a built-in disassembler was handy,
but the GoodWatch10 lacked a radio.  Another round of PCB
manufacturing delivered the GoodWatch20, with a functional radio
transceiver filtered for the 70cm (433MHz band).  Out of band
operation also works, with the expected losses to filtering.

{{% img src="images/gw20-pins.jpg" w="600" %}}

Full hardware CAD and source code are available on Github.  PCBs will
be available soon.

[https://github.com/travisgoodspeed/goodwatch/](https://github.com/travisgoodspeed/goodwatch/)


Your neighbor,

--Travis KK4VCZ
