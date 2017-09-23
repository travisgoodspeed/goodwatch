
#Standard Debian gcc-msp430 and msp430mcu packages.
CC = msp430-gcc -mmcu=cc430f6137

modules=main.o lcd.o

all: goodwatch.hex

goodwatch: $(modules)
	$(CC) -o goodwatch $(modules)
goodwatch.hex: goodwatch
	 msp430-objcopy -O ihex main main.hex

clean:
	rm -f *~ *.hex *.elf *.o goodwatch

erase:
	python -m msp430.bsl5.uart -p /dev/ttyUSB0 --invert-test --invert-reset  -v -e

