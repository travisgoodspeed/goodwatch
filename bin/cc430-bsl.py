#!/usr/bin/env python2


## This is a quick and dirty BSL client by Travis Goodspeed for the
## CC430F6137, written out of frustration when another tool generated
## bad checksums.  It is free for any use, provided you tip your
## bartender.

import serial, time, sys, argparse, string, struct

class BSL:
    def __init__(self, port):
        print("Opening %s" % port)
        self.serial = serial.Serial(port,
                                  baudrate=9600,
                                  parity=serial.PARITY_EVEN,
                                  stopbits=serial.STOPBITS_ONE,
                                  timeout=1)

    #Low level functions first.
    def setTST(self, level):
        """Sets the TST pin."""
        self.serial.setRTS(level)
        time.sleep(0.01)

    def setRST(self, level):
        """Sets the !RST pin."""
        self.serial.setDTR(level)
        time.sleep(0.01)

    def enter_bsl(self):
        """Activates the bootloader by the sequence in SLAU319N."""
        self.setRST(False)
        self.setTST(False)
        time.sleep(0.250)
        self.setRST(True)
        
        #Minimum two rising edges are required, but it helps to have three or more.
        for i in range(4):
            self.setTST(False)
            self.setTST(True)
        
        self.setRST(False)
        self.setTST(False)
        time.sleep(0.250)
        self.serial.flushInput()

    def reset(self):
        """Exits the BSL by resetting the chip."""
        self.setTST(True)
        
        self.setRST(True)
        self.setRST(False)
        self.setRST(True)

    def crc(self, msg):
        """Returns a two-byte string of the checksum of a message."""
        crc = 0xFFFF
        
        #msg should already include header bytes.
        for char in msg:
            byte = ord(char)
            x = ((crc >> 8) ^ byte) & 0xFF
            x ^= x >> 4
            crc = (crc << 8) ^ (x << 12) ^ (x << 5) ^ x
        return struct.pack('<H', crc & 0xFFFF)

    def packadr(adr):
        return struct.pack('<I', adr)[:-1]

    def transact(self, msg):
        """Sends a message, wrapped with a prefix and checksum.
        Result's wrapper is stripped."""

        #Send the message.
        length = len(msg)
        crc = self.crc(msg)
        self.serial.write(struct.pack('<BH', "\x80", length) + msg + crc)

        #Get the reply.
        reply = self.serial.read(1)
        if len(reply) != 1:
            print "Error, missing reply."
            sys.exit(1)
        elif ord(reply[0]) == 0x00:
            #Success
            eighty = ord(self.serial.read(1))
            length = struct.unpack('<H', ''.join(self.serial.read(2))[0]
            rep  =  self.serial.read(length)
            crc = self.serial.read(2)
            assert(crc == self.crc(rep))
            return rep
        else:
            print "Error 0x%02x." % ord(reply[0])
            #Not sure whether data is coming, so grab a chunk just in case.
            self.serial.read(10)

    def unlock(self, password="\xff" * 32):
        """Unlocks the bootloader, optionally with a password."""

        #Password must be 32 bytes.
        assert(len(password) == 32)

        resp = self.transact("\x11" + password)
        assert(len(resp) == 2)
        assert(resp[0] == '\x3b')

        code = ord(resp[1])
    
        if code == 0x05:
            print "Incorrect password.  Flash was erased."
            return self.unlock()
        elif code == 0x00:
            #success
            return True
        else:
            print "Unexpected password core message 0x%02x." % code
            return False
        
    def version(self):
        """Gets the BSL version and related bytes."""
        resp = self.transact("\x19")
        assert(len(resp) == 5)

        # vendor is 0 for TI
        # api is 0 for flash, 30 for sram, 80 for restricted cmd set
        vendor, interpreter, api, peripheral = struct.unpack('<x4B', resp)
        return vendor, interpreter, api, peripheral

    def masserase(self):
        """Bulk erases the device."""
        resp = self.transact("\x15")
        assert resp == "\x3b\x00"

    def read(self, adr, length=32):
        """Dumps memory from the given address."""
        resp = self.transact(struct.pack('<B3sH', "\x18", packadr(adr), length))
        if resp[0] == "\x3b":
            print "Error: You need to unlock before reading."
        assert(resp[0] == "\x3a")
        return resp[1:]

    def write(self, adr, data=""):
        """Writes memory to the given address."""
        resp = self.transact(struct.pack('<B3s',"\x10", packadr(adr)) + data)
        return resp[1:]

    
    MAXLEN = 256 #Maximum bytes per read request.
    def readbulk(self, adr, length):
        """Reads a large volume from the target, in multiple transactions."""
        i = adr
        buf = ""
        while i < adr + length:
            buf = buf + self.read(i, min(self.MAXLEN, adr + length - i))
            i = adr + len(buf)
        assert(len(buf) == length)
        return buf

    def writebulk(self, adr, data):
        """Writes a large volume to the target, in multiple transactions."""
        length = len(data)
        i = adr
        while i < adr + length:
            thislength = min(self.MAXLEN, adr + length - i)
            self.write(i, data[i - adr:i - adr + thislength])
            i = i + thislength

        #assert(i == adr + length)
        return True

    def writeihexline(self, line):
        """Writes one line of an Intel Hex file.  (Langsec sin!)"""
        assert(line[0] == ':')
        length = int(line[1:3], 16)
        adr = int(line[3:7], 16)
        verb = int(line[7:9], 16)
        
        data = line[9:(9 + length * 2)].decode('hex')
        if verb == 0: # Data
            self.write(adr, data)

    def writeihexfile(self, filename):
        """Writes an Intel Hex file to the CC430."""
        f = open(filename, 'r')
        for l in f:
            self.writeihexline(l.strip())

def coredump(bsl):
    """Prints all of memory."""

    print "\n\n\nDumping most of memory as a read test."
    
    ##Dump the BSL
    bulk = bsl.readbulk(0x1000, 2048)
    print "Got %d bytes of the BSL." % len(bulk)
    print bulk.encode('hex')

    ##Dump Info
    bulk = bsl.readbulk(0x1800, 512)
    print "Got %d bytes of Info Flash." % len(bulk)
    print bulk.encode('hex')

    ##Dump RAM
    bulk = bsl.readbulk(0x1C00, 4096)
    print "Got %d bytes of RAM." % len(bulk)
    print bulk.encode('hex')

    ##Dump ROM
    print "Dumping Flash ROM.  Please be patient."
    bulk = bsl.readbulk(0x8000, 32 * 1024)
    print "Got %d bytes of Flash ROM." % len(bulk)
    print bulk.encode('hex')

def writetest(bsl):
    """Tests writing an image to Flash."""

    print "\n\n\nMass erasing, then performing a write test."
    bsl.masserase()
    
    msg = "Hello world!  Hogy vagy?  Jo vagyok!" * 35
    bsl.writebulk(0x8000, msg)
    readmsg = bsl.readbulk(0x8000,len(msg))
    print "Wrote: %s" % msg.encode('hex')
    print "Read:  %s" % readmsg.encode('hex')
    assert(readmsg == msg)


# BSLException, Segment, and Memory are from goodfet.bsl
class BSLException(Exception):
    pass

class Segment:
    """store a string with memory contents along with its startaddress"""
    def __init__(self, startaddress=0, data=None):
        if data is None:
            self.data = ''
        else:
            self.data = data
        self.startaddress = startaddress

    def __getitem__(self, index):
        return self.data[index]

    def __len__(self):
        return len(self.data)

    def __repr__(self):
        return "Segment(startaddress=0x%04x, data=%r)" % (self.startaddress, self.data)

class Memory:
    """represent memory contents. with functions to load files"""
    def __init__(self, filename=None):
        self.segments = []
        if filename:
            self.filename = filename
            self.loadFile(filename)

    def append(self, seg):
        self.segments.append(seg)

    def __getitem__(self, index):
        return self.segments[index]

    def __len__(self):
        return len(self.segments)

    def loadIHex(self, file):
        """load data from a (opened) file in Intel-HEX format"""
        segmentdata = []
        currentAddr = 0
        startAddr   = 0
        lines = file.readlines()
        for l in lines:
            if l[0] != ':': raise BSLException("File Format Error\n")
            l = l.strip()       #fix CR-LF issues...
            length  = int(l[1:3], 16)
            address = int(l[3:7], 16)
            type    = int(l[7:9], 16)
            check   = int(l[-2:], 16)
            if type == 0x00:
                if currentAddr != address:
                    if segmentdata:
                        self.segments.append(Segment(startAddr, string.join(segmentdata, '')))
                    startAddr = currentAddr = address
                    segmentdata = []
                for i in range(length):
                    segmentdata.append(chr(int(l[9 + 2 * i:11 + 2 * i], 16)))
                currentAddr = length + currentAddr
            elif type in (0x01, 0x02, 0x03, 0x04, 0x05):
                pass
            else:
                sys.stderr.write("Ignored unknown field (type 0x%02x) in ihex file.\n" % type)
        if segmentdata:
            self.segments.append(Segment(startAddr, string.join(segmentdata, '')))

    def loadTIText(self, file):
        """load data from a (opened) file in TI-Text format"""
        next        = 1
        startAddr   = 0
        segmentdata = []
        #Convert data for MSP430, TXT-File is parsed line by line
        while next >= 1:
            #Read one line
            l = file.readline()
            if not l: break #EOF
            l = l.strip()
            if l[0] == 'q': break
            elif l[0] == '@':        #if @ => new address => send frame and set new addr.
                #create a new segment
                if segmentdata:
                    self.segments.append(Segment(startAddr, string.join(segmentdata, '')))
                startAddr = int(l[1:], 16)
                segmentdata = []
            else:
                for i in string.split(l):
                    segmentdata.append(chr(int(i,16)))
        if segmentdata:
            self.segments.append(Segment(startAddr, string.join(segmentdata, '')))

    def loadELF(self, file):
        """load data from a (opened) file in ELF object format.
        File must be seekable"""
        import elf
        obj = elf.ELFObject()
        obj.fromFile(file)
        if obj.e_type != elf.ELFObject.ET_EXEC:
            raise Exception("No executable")
        for section in obj.getSections():
            if DEBUG:
                sys.stderr.write("ELF section %s at 0x%04x %d bytes\n" % (section.name, section.lma, len(section.data)))
            if len(section.data):
                self.segments.append(Segment(section.lma, section.data))

    def loadString(self, startAddr=0, string=None):
        """fill memory with the contents of a binary chunk of data."""
        self.segments.append(Segment(startAddr, string))

    def loadFile(self, filename):
        """fill memory with the contents of a file. file type is determined from extension"""
        #TODO: do a contents based detection
        if filename[-4:].lower() == '.txt':
            self.loadTIText(open(filename, "rb"))
        elif filename[-4:].lower() in ('.a43', '.hex'):
            self.loadIHex(open(filename, "rb"))
        else:
            self.loadELF(open(filename, "rb"))

    def getMemrange(self, fromadr, toadr):
        """get a range of bytes from the memory. unavailable values are filled with 0xff."""
        res = ''
        toadr = toadr + 1   #python indxes are excluding end, so include it
        while fromadr < toadr:
            #print "fromto: %04x %04x" % (fromadr, toadr)
            for seg in self.segments:
                #print seg
                segend = seg.startaddress + len(seg.data)
                if seg.startaddress <= fromadr and fromadr < segend:
                    #print "startok 0x%04x %d" % (seg.startaddress, len(seg.data))
                    #print ("0x%04x "*3) % (segend, fromadr, toadr)
                    if toadr > segend:   #not all data in segment
                        #print "out of segment"
                        catchlength = segend-fromadr
                    else:
                        catchlength = toadr-fromadr
                    #print toadr-fromadr
                    #print catchlength
                    res = res + seg.data[fromadr-seg.startaddress : fromadr-seg.startaddress+catchlength]
                    fromadr = fromadr + catchlength    #adjust start
                    if len(res) >= toadr-fromadr:
                        break#return res
            else:
                    res = res + chr(255)
                    fromadr = fromadr + 1 #adjust start
                    #print "fill FF"
        #print "res: %r" % res
        return res


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='CC430F6137 BSL Client')
    parser.add_argument('-e', '--erase', help='Mass Erase', action='store_true')
    parser.add_argument('-p', '--port', help='Serial Port', default='/dev/ttyUSB0')
    parser.add_argument('-f', '--file', help='Flash File')
    parser.add_argument('-P', '--password', help='Password File or Hex')
    parser.add_argument('-d', '--dump', help='Produce a core dump.', action='store_true')
    parser.add_argument('-u', '--unlock', help='Unlock BSL.', action='store_true')
    parser.add_argument('-t', '--time', help='Set the Time.', action='store_true')
    
    args = parser.parse_args()

    bsl=BSL(args.port)
    bsl.enter_bsl()

    if args.erase:
        print "Mass erasing."
        bsl.masserase()
        bsl.unlock()
    if args.unlock:
        print "Unlocking."
        if args.password:
            password_is_file = False
            try:
                password = args.password.decode('hex')
                if len(password) != 32:
                    print "Password is not 32 bytes long, assuming file path"
                    password_is_file = True
            except TypeError:
                print "Non-hexadecimal digit in password, assuming file path"
                password_is_file = True
                
            if password_is_file:
                password = Memory(args.password).getMemrange(0xffe0, 0xffff)
                
            bsl.unlock(password)
        else:
            bsl.unlock()
    if args.file:
        print "Writing %s as Intel hex." % args.file
        bsl.writeihexfile(args.file)
    if args.time:
        lt = time.localtime()
        #See firmware/rtc.c for the format.
        timestr = (
            #Hour, Minute, Second first.
            chr(lt.tm_hour)+chr(lt.tm_min)+chr(lt.tm_sec)+"\xFF"+
            #u16 Year, u8 Month, u8 Day
            chr(lt.tm_year&0xFF)+chr(lt.tm_year>>8)+chr(lt.tm_mon)+chr(lt.tm_mday)
            )
        bsl.write(0xFF00,timestr)
    if args.dump:
        coredump(bsl)
    bsl.reset()
