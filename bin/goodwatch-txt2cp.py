#!/usr/bin/python2

## This quick and dirty tool converts a textfile of frequencies and
## names into a GoodWatch codeplug, for storage in information flash
## at 0x1800.  Use cc430-bsl.py to write the codeplug to flash.

import sys, argparse;


def freqbytes(freq):
    """Converts a frequency to three FREQ bytes.  Assumes 26MHz xtal."""
    freqMult = (0x10000 / 1000000.0) / 26.0;
    num=int(freq*1e6*freqMult);
    FREQ2=(num>>16) & 0xFF;
    FREQ1=(num>> 8) & 0xFF;
    FREQ0= num      & 0xFF
    return FREQ2, FREQ1, FREQ0;

codeplugadr=0x1800;
def handleline(line):
    """Handles one line of the file."""
    global codeplugadr;
    if len(line)==0:   #Empty line.
        return;
    elif line[0]=='#': #Comment
        return;
    else:
        # This is a real line.  First word ought to be the frequency,
        # and the second word ought to be the name (8 characters or
        # less).  Further words would be flags, but we ignore them for
        # now.
        LEN=12;
        words=line.split();
        freq=float(words[0]);
        name=words[1];
        namehex=name.encode('hex');
        assert(len(name)<=8);
        while len(namehex)<16:
            namehex='20'+namehex;
        FLAGS=0;
        (FREQ2, FREQ1, FREQ0) = freqbytes(freq);
        CRC=0xFF;  #FIXME This checksum is wrong, but cc430-bsl.py doesn't bother to check.
        codeplugadr=codeplugadr+12;
        return(":%02x%04x00%02x%02x%02x%02x%s%02x\n" %(
            LEN, codeplugadr, FLAGS, FREQ2,FREQ1,FREQ0, namehex, CRC
            ));
    
def convertcodeplug(infile, outfile):
    """Converts a codeplug textfile into an intel hex file for flashing."""
    i=open(infile,'r');
    o=open(outfile,'w');
    for line in i:
        ihl=handleline(line.strip());
        if(ihl!=None):
            o.write(ihl);
    o.write(":00000001FF\n");
    i.close();
    o.close();


if __name__=='__main__':
    parser = argparse.ArgumentParser(description='GoodWatch Codeplug Compiler')
    parser.add_argument('-i','--input', help='Input Textfile');
    parser.add_argument('-o','--output', help='Output Intel Hex File');
    
    args=parser.parse_args();

    if args.input!=None and args.output!=None:
        convertcodeplug(args.input, args.output);
