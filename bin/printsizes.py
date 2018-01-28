#!/usr/bin/python2

## Quick little pyelftools script to test the size of a CC430F6137
## firmware image, warning the user if it looks too big.

from __future__ import print_function
import sys


from elftools.elf.elffile import ELFFile
from elftools.elf.sections import SymbolTableSection


def process_file(filename):
    with open(filename, 'rb') as f:
        printsizes(f)


def printsizes(stream):
    elffile = ELFFile(stream);

    text=elffile.get_section_by_name('.text');
    textlen=len(text.data());
    codeperc=textlen*100.0/32768;
    print("%d bytes of .text (%d %%)" % (textlen,codeperc));
    if codeperc>80:
        print("WARNING: %d percent of code is used!"%codeperc);
    

    data=elffile.get_section_by_name('.data');
    datalen=len(data.data());
    bss=elffile.get_section_by_name('.bss');
    bsslen=len(bss.data());
    noinit=elffile.get_section_by_name('.noinit');
    noinitlen=len(noinit.data());
    dataperc=(datalen+bsslen+noinitlen)*100.0/4096;
    print("%d bytes of .data, %d bytes of .bss, %d bytes of .noinit (%d %%)"
          %(datalen,bsslen,noinitlen,dataperc));
    if dataperc>80:
        print("WARNING: %d percent of data is used!"%dataperc);


if __name__ == '__main__':
    for filename in sys.argv[1:]:
        process_file(filename)
