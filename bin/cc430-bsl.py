#!/usr/bin/python2


## This is a quick and dirty BSL client by Travis Goodspeed for the
## CC430F6137, written out of frustration when another tool generated
## bad checksums.  It is free for any use, provided you tip your
## bartender.

import serial, time, sys, argparse, progressbar;


class BSL:
    def __init__(self, port):
        #print("Opening %s" % port);
        self.serial=serial.Serial(port,
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
        self.setTST(False)
        self.setRST(True)
        time.sleep(0.10)

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
        time.sleep(0.10)
        self.serial.flushInput();
    def reset(self):
        """Exits the BSL by resetting the chip."""
        self.setTST(True)
        
        self.setRST(True);
        self.setRST(False);
        self.setRST(True);
            
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
            print("Error, missing reply.");
            sys.exit(1);
        elif msg[0]=='\x52':
            #Change baud rate command has a briefer reply.
            pass;
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
            print("Error 0x%02x." % ord(reply[0]));
            #Not sure whether data is coming, so grab a chunk just in case.
            self.serial.read(10);
    
    def unlock(self,
               password=None):
        """Unlocks the bootloader, optionally with a password."""
        

        #Password must be 32 bytes; read from a file otherwise.
        if password==None:
            password="\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff"
        if len(password)!=32:
            password=self.passwordfromfile(password);

        assert(len(password)==32);

        resp=self.transact("\x11"+password);
        assert(len(resp)==2);
        assert(resp[0]=='\x3b');

        code=ord(resp[1]);
    
        if code==0x05:
            print("Incorrect password.  Flash was erased.");
            return self.unlock();
        elif code==0x00:
            #success
            return True;
        else:
            print("Unexpected password core message 0x%02x."%code);
            return False;
        
    def version(self):
        """Gets the BSL version and related bytes."""
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
        if resp[0]=="\x3b":
            print("Error: You need to unlock before reading.");
        assert(resp[0]=="\x3a");
        
        return resp[1:]
    def write(self,adr,data=""):
        """Writes memory to the given address."""
        al=chr(adr&0xFF)
        am=chr((adr>>8)&0xFF)
        ah=chr((adr>>16)&0xFF)
        resp=self.transact("\x10"+al+am+ah+data)
        return resp[1:]

    def erasesegment(self,adr):
        """Erases one segment of Flash at a given address."""
        al=chr(adr&0xFF)
        am=chr((adr>>8)&0xFF)
        ah=chr((adr>>16)&0xFF)
        resp=self.transact("\x12"+al+am+ah)
        return resp[1:]
    
    def unlocklockinfo(self):
        """Unlocks or locks Info FLash."""
        resp=self.transact("\x13")
        return resp[1:]

    def loadpc(self,adr):
        """Begins execution at a given address."""
        al=chr(adr&0xFF)
        am=chr((adr>>8)&0xFF)
        ah=chr((adr>>16)&0xFF)
        resp=self.transact("\x17"+al+am+ah)
        return resp[1:]
    
    def setbaud(self,rate=9600):
        """Sets the baud rate."""
        #First we inform the BSL of the rate.
        if rate==9600:
            ratebyte='\x02';
        elif rate==19200:
            ratebyte='\x03';
        elif rate==38400:
            ratebyte='\x04';
        elif rate==57600:
            ratebyte='\x05';
        elif rate==115200:
            ratebyte='\x06';

        #Command the rate change.
        resp=self.transact("\x52"+ratebyte);
        
        #Then we jump the port to the new rate.
        #self.serial.setBaudrate(rate); #Old convention.
        self.serial.baudrate=rate;      #New convention.
    
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

    def writeihexline(self,line):
        """Writes one line of an Intel Hex file.  (Langsec sin!)"""
        assert(line[0]==':');
        length=int(line[1:3],16);
        adr=int(line[3:7],16);
        verb=int(line[7:9],16);
        
        data=line[9:(9+length*2)].decode('hex');
        if verb==0: #Data
            self.write(adr,data);

    def writeihexfile(self,filename):
        """Writes an Intel Hex file to the CC430."""
        bar=progressbar.ProgressBar();
        f=open(filename,'r');
        lines=f.readlines();
        for l in bar(lines):
            self.writeihexline(l.strip());


    def handlepasswordline(self,line):
        """Returns a password fragment from the line, if available."""
        assert(line[0]==':');
        length=int(line[1:3],16);
        adr=int(line[3:7],16);
        verb=int(line[7:9],16);
        
        data=line[9:(9+length*2)].decode('hex');
        if verb==0 and adr>=0xFFE0:
            #Return the password bytes from this line.
            return data;

        #Empty string by default.
        return "";

    def passwordfromfile(self,filename):
        """Returns the password from an ihex file, for unlocking."""
        f=open(filename,'r');
        lines=f.readlines();
        password="";
        for l in lines:
            password+=self.handlepasswordline(l);
        return password;
    
def coredump(bsl):
    """Prints all of memory."""

    print("\n\n\nDumping most of memory as a read test.");
    
    ##Dump the BSL
    bulk=bsl.readbulk(0x1000,2048)
    print("Got %d bytes of the BSL." % len(bulk));
    print(bulk.encode('hex'));

    ##Dump Info
    bulk=bsl.readbulk(0x1800,512)
    print("Got %d bytes of Info Flash." % len(bulk));
    print(bulk.encode('hex'));

    ##Dump RAM
    bulk=bsl.readbulk(0x1C00,4096)
    print("Got %d bytes of RAM." % len(bulk));
    print(bulk.encode('hex'));

    ##Dump ROM
    print("Dumping Flash ROM.  Please be patient.")
    bulk=bsl.readbulk(0x8000,32*1024)
    print("Got %d bytes of Flash ROM." % len(bulk));
    print(bulk.encode('hex'));

def dmesg(bsl):
    """Prints the dmesg buffer."""

    ##Dump RAM
    bulk=bsl.readbulk(0x2400,2048)
    print(bulk);

def writetest(bsl):
    """Tests writing an image to Flash."""

    print("\n\n\nMass erasing, then performing a write test.");
    bsl.masserase();
    
    msg="Hello world!  Hogy vagy?  Jo vagyok!"*35
    bsl.writebulk(0x8000,msg);
    readmsg=bsl.readbulk(0x8000,len(msg));
    print("Wrote: %s" % msg.encode('hex'));
    print("Read:  %s" % readmsg.encode('hex'));
    assert(readmsg==msg);

if __name__=='__main__':
    parser = argparse.ArgumentParser(description='CC430F6137 BSL Client')
    parser.add_argument('-e','--erase', help='Mass Erase (w/o Info)',action='count');
    parser.add_argument('-E','--eraseinfo', help='Mass Erase (w/  Info)',action='count');
    parser.add_argument('-p','--port',
                        help='Serial Port',default='/dev/ttyUSB0');
    parser.add_argument('-f','--file', help='Flash File');
    parser.add_argument('-P','--password', help='Password File or Hex');
    parser.add_argument('-d','--dump',
                        help='Produce a core dump.',action='count');
    parser.add_argument('-D','--dmesg',
                        help='Prints the dmesg.',action='count');
    parser.add_argument('-u','--unlock',
                        help='Unlock BSL.',action='count');
    parser.add_argument('-r','--rate',
                        help='Baud Rate', default=9600);
    
    args = parser.parse_args()

    bsl=BSL(args.port);
    bsl.enter_bsl();
    
    if int(args.rate)!=9600:
        print("Setting baud rate to %d"% int(args.rate));
        bsl.setbaud(int(args.rate));

    if args.erase!=None:
        print("Mass erasing.")
        bsl.masserase();
        bsl.unlock();
    if args.eraseinfo!=None:
        print("Mass erasing, plus Info.")
        bsl.masserase();
        bsl.unlock();
        #We don't need to call unlocklockinfo() because we are
        #targetting the banks directly.
        bsl.erasesegment(0x1800);
        bsl.erasesegment(0x1880);
        bsl.erasesegment(0x1900);
        bsl.erasesegment(0x1980);
        
    if args.unlock!=None:
        #print "Unlocking."
        bsl.unlock(args.password);
    if args.file!=None:
        print("Writing %s as Intel hex." % args.file)
        bsl.writeihexfile(args.file);

    ## Peviously, we manually wrote the time to 0xFF00.  This is now
    ## handled by buildtime.h, but I wouldn't object to writing the
    ## time to SRAM. --Travis
    
    # if args.time!=None:
    #     lt=time.localtime()
    #     #See firmware/rtc.c for the format.
    #     timestr=(
    #         #Hour, Minute, Second first.
    #         chr(lt.tm_hour)+chr(lt.tm_min)+chr(lt.tm_sec)+"\xFF"+
    #         #u16 Year, u8 Month, u8 Day
    #         chr(lt.tm_year&0xFF)+chr(lt.tm_year>>8)+chr(lt.tm_mon)+chr(lt.tm_mday)
    #         );
    #     bsl.write(0xFF00,timestr);
    
    if args.dump!=None:
        coredump(bsl);
    if args.dmesg!=None:
        dmesg(bsl);
    bsl.reset();
    
