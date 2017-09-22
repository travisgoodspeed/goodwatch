#!/usr/bin/python2


## This is a quick and dirty BSL client by Travis Goodspeed for the
## CC430F6137, written out of frustration when another tool generated
## bad checksums.  It is free for any use, provided you tip your
## bartender.

import serial, time, sys;


class BSL:
    def __init__(self, port):
        print("Opening %s" % port);
        self.serial=serial.Serial("/dev/ttyUSB0",
                                  baudrate=9600,
                                  parity=serial.PARITY_EVEN,
                                  stopbits=serial.STOPBITS_ONE,
                                  timeout=1);

    #Low level functions first.
    def setTST(self,level):
        """Sets the TST pin."""
        self.serial.setRTS(level)
        time.sleep(0.01);
    def setRST(self,level):
        """Sets the !RST pin."""
        self.serial.setDTR(level);
        time.sleep(0.01);

    def enter_bsl(self):
        """Activates the bootloader by the sequence in SLAU319N."""
        self.setRST(False)
        self.setTST(False)
        time.sleep(0.250)
        self.setRST(True)

        #Minimum two rising edges are required, but it helps to have three or more.
        self.setTST(False)
        self.setTST(True)
        self.setTST(False)
        self.setTST(True)
        self.setTST(False)
        self.setTST(True)
        self.setTST(False)
        self.setTST(True)
        
        self.setRST(False)
        self.setTST(False)
        time.sleep(0.250)
        self.serial.flushInput();

    def crc(self,msg):
        """Returns a two-byte string of the checksum of a message."""
        crc=0xFFFF
        
        #msg should already include header bytes.
        for char in msg:
            byte=ord(char)
            x=((crc>>8)^byte)&0xFF;
            x^=x>>4;
            crc=(crc<<8)^(x<<12)^(x<<5)^x;
        return chr(crc&0xFF)+""+chr((crc>>8)&0xFF);

    def transact(self,msg):
        """Sends a message, wrapped with a prefix and checksum.
        Result's wrapper is stripped."""

        #Send the message.
        length=len(msg);
        ll=chr(length&0xFF);
        lh=chr((length>>8)&0xFF);
        crc=self.crc(msg);
        self.serial.write("\x80"+ll+lh+msg+crc);

        #Get the reply.
        reply=self.serial.read(1);
        if len(reply)!=1:
            print "Error, missing reply.";
            sys.exit(1);
        elif ord(reply[0])==0x00:
            #Success
            eighty=ord(self.serial.read(1));
            ll=ord(self.serial.read(1)[0]);
            lh=ord(self.serial.read(1)[0]);
            length=ll|(lh<<8);
            rep=self.serial.read(length);
            crc=self.serial.read(2);
            assert(crc==self.crc(rep));
            return rep;
        else:
            print "Error 0x%02x." % ord(reply[0]);
            #Not sure whether data is coming, so grab a chunk just in case.
            self.serial.read(10);
    
    def unlock(self,
               password="\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff"):
        """Unlocks the bootloader, optionally with a password."""

        #Password must be 32 bytes.
        assert(len(password)==32);

        resp=self.transact("\x11"+password);
        assert(len(resp)==2);
        assert(resp[0]=='\x3b');

        code=ord(resp[1]);
    
        if code==0x05:
            print "Error, incorrect password."
            return False;
        elif code==0x00:
            #success
            return True;
        else:
            print "Unexpected password core message 0x%02x."%code;
            return False;
        
    def version(self):
        """Gets the BSL version and related bytes."""
        #self.serial.write("\x80\x01\x00\x19"+self.crc("\x80\x01\x00\x19"));
        resp=self.transact("\x19");
        assert(len(resp)==5);
        
        vendor=ord(resp[1]); # 0 for TI
        interpreter=ord(resp[2]);
        api=ord(resp[3]); # 0 for flash, 30 for sram, 80 for restricted cmd set
        peripheral=ord(resp[4]);
        return vendor, interpreter, api, peripheral;

    def masserase(self):
        """Bulk erases the device."""
        resp=self.transact("\x15");
        assert resp=="\x3b\x00"

    def read(self,adr,length=32):
        """Dumps memory from the given address."""
        al=chr(adr&0xFF)
        am=chr((adr>>8)&0xFF)
        ah=chr((adr>>16)&0xFF)
        ll=chr(length&0xFF)
        lh=chr((length>>8)&0xFF)
        resp=self.transact("\x18"+al+am+ah+ll+lh)
        return resp[1:]
    def write(self,adr,data=""):
        """Writes memory to the given address."""
        al=chr(adr&0xFF)
        am=chr((adr>>8)&0xFF)
        ah=chr((adr>>16)&0xFF)
        resp=self.transact("\x10"+al+am+ah+data)
        return resp[1:]

    
    MAXLEN=256; #Maximum bytes per read request.
    def readbulk(self,adr,length):
        """Reads a large volume from the target, in multiple transactions."""
        i=adr;
        buf="";
        while i<adr+length:
            buf=buf+self.read(i,min(self.MAXLEN,adr+length-i));
            i=adr+len(buf);
        assert(len(buf)==length);
        return buf;
    def writebulk(self,adr,data):
        """Writes a large volume to the target, in multiple transactions."""
        length=len(data);
        i=adr;
        while i<adr+length:
            thislength=min(self.MAXLEN,adr+length-i);
            self.write(i,data[i-adr:i-adr+thislength]);
            i=i+thislength;

        #assert(i==adr+length);
        return True;




def coredump(bsl):
    """Prints all of memory."""

    print "\n\n\nDumping most of memory as a read test.";
    
    ##Dump the BSL
    bulk=bsl.readbulk(0x1000,2048)
    print "Got %d bytes of the BSL." % len(bulk);
    print bulk.encode('hex');

    ##Dump Info
    bulk=bsl.readbulk(0x1800,512)
    print "Got %d bytes of Info Flash." % len(bulk);
    print bulk.encode('hex');

    ##Dump RAM
    bulk=bsl.readbulk(0x1C00,4096)
    print "Got %d bytes of RAM." % len(bulk);
    print bulk.encode('hex');

    ##Dump ROM
    print "Dumping Flash ROM.  Please be patient."
    bulk=bsl.readbulk(0x8000,32*1024)
    print "Got %d bytes of Flash ROM." % len(bulk);
    print bulk.encode('hex');

def writetest(bsl):
    """Tests writing an image to Flash."""

    print "\n\n\nMass erasing, then performing a write test.";
    bsl.masserase();
    
    msg="Hello world!  Hogy vagy?  Jo vagyok!"*35
    bsl.writebulk(0x8000,msg);
    readmsg=bsl.readbulk(0x8000,len(msg));
    print "Wrote: %s" % msg.encode('hex');
    print "Read:  %s" % readmsg.encode('hex');
    assert(readmsg==msg);

if __name__=='__main__':
    bsl=BSL("/dev/ttyUSB0");

    print "Testing the BSL library:"

    bsl.enter_bsl();

    if not bsl.unlock():
        print "Unable to unlock BSL.  Try a bulk erase."
        sys.exit(1);

    bsl.version();
    coredump(bsl);
    writetest(bsl);
    
    
