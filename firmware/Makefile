
#set default flashing serial port, dont override if passed in as an argument
PORT = /dev/ttyUSB0

#Standard Debian gcc-msp430 and msp430mcu packages.
CC = msp430-gcc -mmcu=cc430f6137 -Wall -I. -Os

BSL = ../bin/cc430-bsl.py -r 38400 -p $(PORT)

modules=rtcasm.o main.o lcd.o lcdtext.o rtc.o  keypad.o apps.o applist.o \
	sidebutton.o power.o uart.o monitor.o ucs.o buzz.o \
	radio.o packet.o dmesg.o codeplug.o rng.o \
	libs/assembler.o libs/morse.o 

apps= apps/clock.o apps/rpn.o apps/hex.o apps/stopwatch.o apps/dice.o \
	apps/rngapp.o apps/tuner.o apps/morse.o apps/beacon.o apps/iclicker.o \
	apps/submenu.o

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
	$(BSL) -P goodwatch.hex -uD

#Same as dmesg, but it gives the target some time to boot first.
run:
	sleep 5
	../bin/cc430-bsl.py -p $(PORT) -P goodwatch.hex -uD

codeplug.hex: codeplug.txt
	../bin/goodwatch-txt2cp.py -i codeplug.txt -o codeplug.hex
flashcp: codeplug.hex
	#FIXME Might need a different sort of erase.
	$(BSL) -Ef codeplug.hex
dump:
	$(BSL) -ed


