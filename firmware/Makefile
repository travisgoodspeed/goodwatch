
#Standard Debian gcc-msp430 and msp430mcu packages.
CC = msp430-gcc -mmcu=cc430f6137 -Wall -I.

BSL = ../bin/cc430-bsl.py

modules=main.o lcd.o lcdtext.o rtc.o keypad.o apps.c
apps= apps/clock.o apps/rpn.o

all: goodwatch.hex

goodwatch: $(modules) $(apps)
	$(CC) -o goodwatch $(modules) $(apps)
goodwatch.hex: goodwatch
	msp430-objcopy -O ihex goodwatch goodwatch.hex

clean:
	rm -f *~ *.hex *.elf *.o goodwatch

erase:
	$(BSL) -e
flash: goodwatch.hex
	$(BSL) -etf goodwatch.hex


