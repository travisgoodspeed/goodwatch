
#GCC8 from Texas Instruments, not the GCC4 that ships with Debian.
CC = msp430-elf-gcc -minrt -msmall -mmcu=cc430f6137 -Wall -I. -I/opt/msp430-gcc-support-files/include/ -Os  -Wl,--gc-sections,--print-gc-sections -fdata-sections -ffunction-sections -fno-asynchronous-unwind-tables -flto -fshort-enums

modules=main.o radio.o packet.o debug.o flash.o

all: ccode
	

blob.elf: $(modules)  
	$(CC) -T cc430f6137.ld -o blob.elf $(modules) $(apps)
	../../bin/printsizes.py blob.elf || echo "Please install python-pyelftools."

blob.hex: blob.elf
	msp430-elf-objcopy -O ihex blob.elf blob.hex

ccode: blob.hex
	python convert.py blob.hex -i -c
	
debug: CC += -DDEBUG
debug: ccode
	
clean:
	rm -rf *~ */*~ *.hex *.elf *.o */*.o blob.hex blob.elf
