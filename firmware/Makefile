
ALARM_APP = 1
CALIBRATE_APP = 1
RPN_APP = 1
HEX_APP = 1
STOPWATCH_APP = 1
PHRASE_APP = 0
RNG_APP = 1
TUNER_APP = 1
MORSE_APP = 1
BEACON_APP = 1
OOK_APP = 1
COUNTER_APP = 1

#set default flashing serial port, dont override if passed in as an argument
PORT = /dev/ttyUSB0
APPS_OBJ += apps/clock.o apps/submenu.o 

#add optional apps
ifeq ($(ALARM_APP),1)
APPS_OBJ += apps/alarm.o 
APPS_DEFINES += ALARM_APP
endif
ifeq ($(RPN_APP),1)
APPS_OBJ += apps/rpn.o 
APPS_DEFINES += RPN_APP
endif
ifeq ($(HEX_APP),1)
APPS_OBJ += apps/hex.o 
APPS_DEFINES += HEX_APP
endif
ifeq ($(CALIBRATE_APP),1)
APPS_OBJ += apps/calibrate.o 
APPS_DEFINES += CALIBRATE_APP
endif 
ifeq ($(STOPWATCH_APP),1)
APPS_OBJ += apps/stopwatch.o 
APPS_DEFINES += STOPWATCH_APP
endif 
ifeq ($(PHRASE_APP),1)
APPS_OBJ += apps/phrase.o 
APPS_DEFINES += PHRASE_APP
endif
ifeq ($(RNG_APP),1)
APPS_OBJ += apps/rngapp.o 
APPS_DEFINES += RNG_APP
endif 
ifeq ($(TUNER_APP),1)
APPS_OBJ += apps/tuner.o
APPS_DEFINES += TUNER_APP 
endif
ifeq ($(MORSE_APP),1)
APPS_OBJ += apps/morse.o 
APPS_DEFINES += MORSE_APP
endif
ifeq ($(BEACON_APP),1)
APPS_OBJ += apps/beacon.o
APPS_DEFINES += BEACON_APP 
endif
ifeq ($(OOK_APP),1)
APPS_OBJ += apps/ook.o
APPS_DEFINES += OOK_APP 
endif
ifeq ($(COUNTER_APP),1)
APPS_OBJ += apps/counter.o
APPS_DEFINES += COUNTER_APP 
endif


#Standard Debian gcc-msp430 and msp430mcu packages.
CC = msp430-gcc -mmcu=cc430f6137 -Wall -I. -Os  $(addprefix -D, $(APPS_DEFINES))

BSL = ../bin/cc430-bsl.py -r 38400 -p $(PORT)

modules=rtcasm.o main.o lcd.o lcdtext.o rtc.o  keypad.o bcd.o apps.o applist.o adc.o ref.o \
	sidebutton.o power.o uart.o monitor.o ucs.o buzz.o \
	radio.o packet.o dmesg.o codeplug.o rng.o descriptor.o \
	optim.o libs/assembler.o libs/morse.o

apps= $(APPS_OBJ)

all: goodwatch.hex

*.c: githash.h

githash.h:
	echo "#define GITHASH" 0x`git rev-parse HEAD | head -c7` > githash.h
goodwatch: $(modules) $(apps) *.h  
	$(CC) -T msp430.x -o goodwatch $(modules) $(apps)
	../bin/printsizes.py goodwatch || echo "Please install python-pyelftools."

goodwatch.hex: goodwatch
	msp430-objcopy -O ihex goodwatch goodwatch.hex

clean:
	rm -rf *~ */*~ *.hex *.elf *.o */*.o goodwatch githash.h html latex
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


