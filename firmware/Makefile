
#Standard Debian gcc-msp430 and msp430mcu packages.
CC = msp430-gcc -mmcu=cc430f6137 -Wall -I. -Os

BSL = ../bin/cc430-bsl.py

modules=rtcasm.o main.o lcd.o lcdtext.o rtc.o  keypad.o apps.o sidebutton.o radio.o power.o \
	dmesg.o \
	libs/assembler.o
apps= apps/clock.o apps/rpn.o apps/hex.o apps/stopwatch.o apps/morse.c

all: goodwatch.hex

*.c: gittag.h

gittag.h:
	echo "#define GITTAG" 0x`git describe --always` >gittag.h
goodwatch: $(modules) $(apps) *.h
	$(CC) -T msp430.x -o goodwatch $(modules) $(apps)
	../bin/printsizes.py goodwatch || echo "Please install python-pyelftools."

goodwatch.hex: goodwatch
	msp430-objcopy -O ihex goodwatch goodwatch.hex

clean:
	rm -rf *~ */*~ *.hex *.elf *.o */*.o goodwatch gittag.h html latex
erase:
	$(BSL) -e
flash: goodwatch.hex
	$(BSL) -etf goodwatch.hex
dmesg:
	../bin/cc430-bsl.py -P goodwatch.hex -uD

#Same as dmesg, but it gives the target some time to boot first.
run:
	sleep 5
	../bin/cc430-bsl.py -P goodwatch.hex -uD
