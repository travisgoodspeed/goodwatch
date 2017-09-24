
#Standard Debian gcc-msp430 and msp430mcu packages.
CC = msp430-gcc -mmcu=cc430f6137

BSL = ../bin/cc430-bsl.py


modules=main.o lcd.o lcdtext.o

all: goodwatch.hex

goodwatch: $(modules)
	$(CC) -o goodwatch $(modules)
goodwatch.hex: goodwatch
	 msp430-objcopy -O ihex goodwatch goodwatch.hex

clean:
	rm -f *~ *.hex *.elf *.o goodwatch

erase:
	$(BSL) -e
flash: goodwatch.hex
	$(BSL) -ef goodwatch.hex


