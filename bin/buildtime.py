#!/usr/bin/python3


## This is a quick and dirty script for placing the build time into
## the GoodWatch's source code, much as we put the git tag.
## Previously, we just wrote the compile time to memory at 0xff00, but
## that left it out of the ELF file, causing some confusion.

import time, sys;


lt=time.localtime()
#See firmware/rtc.c for the format.
timestr=(
    #Hour, Minute, Second first.
    chr(lt.tm_hour)+chr(lt.tm_min)+chr(lt.tm_sec)+"\xFF"+
    #u16 Year, u8 Month, u8 Day
    chr(lt.tm_year&0xFF)+chr(lt.tm_year>>8)+chr(lt.tm_mon)+chr(lt.tm_mday)
);

sys.stdout.write("#define BUILDTIME \"");
for b in timestr:
    sys.stdout.write("\\x%02x"%ord(b));
sys.stdout.write("\"\n");

