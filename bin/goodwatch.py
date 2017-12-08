#!/usr/bin/python2

## This is a quick and dirty python client for communicating with a
## GoodWatch over its UART.

import serial, time, sys, argparse, progressbar;


def ord16(word):
    """Convert a 16-bit word from a string."""
    return ord(word[0]) | (ord(word[1])<<8)
def chr16(word):
    """Convert a 16-bit word to a string."""
    return chr(word&0xFF)+chr((word>>8)&0xFF);


class GoodWatch:
    def __init__(self, port):
        #print("Opening %s" % port);
        self.serial=serial.Serial(port,
                                  baudrate=9600,
                                  #parity=serial.PARITY_EVEN,
                                  #stopbits=serial.STOPBITS_ONE,
                                  timeout=1);

    def setTST(self,level):
        """Sets the TST pin."""
        self.serial.setRTS(level)
        time.sleep(0.01);
    def setRST(self,level):
        """Sets the !RST pin."""
        self.serial.setDTR(level);
        time.sleep(0.01);
        
    def reset(self):
        """Exits the BSL by resetting the chip."""
        self.setTST(True)
        self.setRST(True);
        self.setRST(False);
        self.setRST(True);
        self.setRST(False);
        time.sleep(1);

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
            #assert(crc==self.crc(rep));
            return rep;
        else:
            print "Error 0x%02x." % ord(reply[0]);
            #Not sure whether data is coming, so grab a chunk just in case.
            self.serial.read(10);
    def turbomode(self,enable=1):
        """Enable turbo mode.  We have to do this slowly because the
        chip is running slowly."""
        #self.transact("\x00"+chr(enable));
        self.serial.write("\x00");
        time.sleep(0.2);
        self.serial.write("\x80");
        time.sleep(0.2);
        self.serial.write("\x02");
        time.sleep(0.2);
        #Command packet.
        self.serial.write("\x00");
        time.sleep(0.2);
        self.serial.write("\x00");
        time.sleep(0.2);
        self.serial.write(chr(enable));
        time.sleep(0.2);
        self.serial.write("\xde");  #TODO Fix this checksum.
        time.sleep(0.2);
        self.serial.write("\xad");
        time.sleep(0.2);
        
        reply=self.serial.read(9);
        
    def peek(self,adr):
        """Peeks a 16-bit word from memory."""
        s=self.transact("\x01\x00"+chr16(adr));
        return ord16(s[2:4]);
        
if __name__=='__main__':
    parser = argparse.ArgumentParser(description='GoodWatch Client')
    parser.add_argument('-p','--port',
                        help='Serial Port',default='/dev/ttyUSB0');
    parser.add_argument('-r','--peek',
                        help='Serial Port');
    
    parser.add_argument('-d','--dump',
                        help='Produce a core dump.',action='count');
    parser.add_argument('-D','--dmesg',
                        help='Prints the dmesg.',action='count');
    
    args = parser.parse_args()

    goodwatch=GoodWatch(args.port);
    goodwatch.reset();
    goodwatch.setRST(False);

    #Switch to turbomode for more reliable comms.
    time.sleep(3);
    try:
        goodwatch.turbomode();
    except:
        print "turbo error."

    if args.peek!=None:
        adr=int(args.peek,16);
        val=goodwatch.peek(adr);
        print "0x%04x: %04x\n" % (adr,val);
    #Exit turbomode when we're done.
    #goodwatch.turbomode(0);
    
