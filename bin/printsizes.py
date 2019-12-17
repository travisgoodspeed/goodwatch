#!/usr/bin/python3

## Quick little pyelftools script to test the size of a CC430F6137
## firmware image, warning the user if it looks too big.

from __future__ import print_function
import sys


from elftools.elf.elffile import ELFFile
from elftools.elf.sections import SymbolTableSection


def process_file(filename):
    print("%s:" % filename);
    with open(filename, 'rb') as f:
        printsizes(f)


def printsizes(stream):
    elffile = ELFFile(stream);

    
    text=elffile.get_section_by_name('.text');
    textlen=len(text.data());
    rodata=elffile.get_section_by_name('.rodata');
    rodatalen=0;
    try: rodatalen=len(rodata.data());
    except: pass;
    codeperc=(textlen+rodatalen)*100.0/32768;
    print("\t%d bytes of .text, %d bytes of .rodata (%d%% Flash)" % (textlen,rodatalen,codeperc));
    if codeperc>90:
        print("WARNING: %d percent of Flash is used!"%codeperc);
    
    datalen=0;
    bsslen=0;
    noinitlen=0;
    
    data=elffile.get_section_by_name('.data');
    datalen=len(data.data());
    bss=elffile.get_section_by_name('.bss');
    bsslen=len(bss.data());
    noinit=elffile.get_section_by_name('.noinit');
    if noinit!=None:
        noinitlen=len(noinit.data());
    dataperc=(datalen+bsslen+noinitlen)*100.0/4096;

    if datalen+bsslen+noinitlen>0:
        print("\t%d bytes of .data, %d bytes of .bss, %d bytes of .noinit (%d%% RAM)"
              %(datalen,bsslen,noinitlen,dataperc));
    if dataperc>80:
        print("WARNING: %d percent of data is used!"%dataperc);


if __name__ == '__main__':
    for filename in sys.argv[1:]:
        process_file(filename)
