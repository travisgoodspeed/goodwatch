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



# Radio Core Registers
IOCFG2              =0x00    #IOCFG2   - GDO2 output pin configuration 
IOCFG1              =0x01    #IOCFG1   - GDO1 output pin configuration 
IOCFG0              =0x02    #IOCFG1   - GDO0 output pin configuration 
FIFOTHR             =0x03    #FIFOTHR  - RX FIFO and TX FIFO thresholds
SYNC1               =0x04    #SYNC1    - Sync word, high byte
SYNC0               =0x05    #SYNC0    - Sync word, low byte
PKTLEN              =0x06    #PKTLEN   - Packet length
PKTCTRL1            =0x07    #PKTCTRL1 - Packet automation control
PKTCTRL0            =0x08    #PKTCTRL0 - Packet automation control
ADDR                =0x09    #ADDR     - Device address
CHANNR              =0x0A    #CHANNR   - Channel number
FSCTRL1             =0x0B    #FSCTRL1  - Frequency synthesizer control
FSCTRL0             =0x0C    #FSCTRL0  - Frequency synthesizer control
FREQ2               =0x0D    #FREQ2    - Frequency control word, high byte
FREQ1               =0x0E    #FREQ1    - Frequency control word, middle byte
FREQ0               =0x0F    #FREQ0    - Frequency control word, low byte
MDMCFG4             =0x10    #MDMCFG4  - Modem configuration
MDMCFG3             =0x11    #MDMCFG3  - Modem configuration
MDMCFG2             =0x12    #MDMCFG2  - Modem configuration
MDMCFG1             =0x13    #MDMCFG1  - Modem configuration
MDMCFG0             =0x14    #MDMCFG0  - Modem configuration
DEVIATN             =0x15    #DEVIATN  - Modem deviation setting
MCSM2               =0x16    #MCSM2    - Main Radio Control State Machine configuration
MCSM1               =0x17    #MCSM1    - Main Radio Control State Machine configuration
MCSM0               =0x18    #MCSM0    - Main Radio Control State Machine configuration
FOCCFG              =0x19    #FOCCFG   - Frequency Offset Compensation configuration
BSCFG               =0x1A    #BSCFG    - Bit Synchronization configuration
AGCCTRL2            =0x1B    #AGCCTRL2 - AGC control
AGCCTRL1            =0x1C    #AGCCTRL1 - AGC control
AGCCTRL0            =0x1D    #AGCCTRL0 - AGC control
WOREVT1             =0x1E    #WOREVT1  - High byte Event0 timeout
WOREVT0             =0x1F    #WOREVT0  - Low byte Event0 timeout
WORCTRL             =0x20    #WORCTRL  - Wake On Radio control
FREND1              =0x21    #FREND1   - Front end RX configuration
FREND0              =0x22    #FREDN0   - Front end TX configuration
FSCAL3              =0x23    #FSCAL3   - Frequency synthesizer calibration
FSCAL2              =0x24    #FSCAL2   - Frequency synthesizer calibration
FSCAL1              =0x25    #FSCAL1   - Frequency synthesizer calibration
FSCAL0              =0x26    #FSCAL0   - Frequency synthesizer calibration
#RCCTRL1             =0x27    #RCCTRL1  - RC oscillator configuration
#RCCTRL0             =0x28    #RCCTRL0  - RC oscillator configuration
FSTEST              =0x29    #FSTEST   - Frequency synthesizer calibration control
PTEST               =0x2A    #PTEST    - Production test
AGCTEST             =0x2B    #AGCTEST  - AGC test
TEST2               =0x2C    #TEST2    - Various test settings
TEST1               =0x2D    #TEST1    - Various test settings
TEST0               =0x2E    #TEST0    - Various test settings

# status registers 
PARTNUM             =0x30    #PARTNUM    - Chip ID
VERSION             =0x31    #VERSION    - Chip ID
FREQEST             =0x32    #FREQEST     Frequency Offset Estimate from demodulator
LQI                 =0x33    #LQI         Demodulator estimate for Link Quality
RSSI                =0x34    #RSSI        Received signal strength indication
MARCSTATE           =0x35    #MARCSTATE   Main Radio Control State Machine state
WORTIME1            =0x36    #WORTIME1    High byte of WOR time
WORTIME0            =0x37    #WORTIME0    Low byte of WOR time
PKTSTATUS           =0x38    #PKTSTATUS   Current GDOx status and packet status
VCO_VC_DAC          =0x39    #VCO_VC_DAC  Current setting from PLL calibration module
TXBYTES             =0x3A    #TXBYTES     Underflow and number of bytes
RXBYTES             =0x3B    #RXBYTES     Overflow and number of bytes

# burst write registers
PATABLE             =0x3E    #PATABLE - PA control settings table
TXFIFO              =0x3F    #TXFIFO  - Transmit FIFO
RXFIFO              =0x3F    #RXFIFO  - Receive FIFO


# Radio Core Instructions
# command strobes              
RF_SRES             =0x30    #SRES    - Reset chip.
RF_SFSTXON          =0x31    #SFSTXON - Enable and calibrate frequency synthesizer.
RF_SXOFF            =0x32    #SXOFF   - Turn off crystal oscillator.
RF_SCAL             =0x33    #SCAL    - Calibrate frequency synthesizer and turn it off.
RF_SRX              =0x34    #SRX     - Enable RX. Perform calibration if enabled.
RF_STX              =0x35    #STX     - Enable TX. If in RX state, only enable TX if CCA passes.
RF_SIDLE            =0x36    #SIDLE   - Exit RX / TX, turn off frequency synthesizer.
#RF_SRSVD            =0x37    #SRVSD   - Reserved.  Do not use.
RF_SWOR             =0x38    #SWOR    - Start automatic RX polling sequence (Wake-on-Radio)
RF_SPWD             =0x39    #SPWD    - Enter power down mode when CSn goes high.
RF_SFRX             =0x3A    #SFRX    - Flush the RX FIFO buffer.
RF_SFTX             =0x3B    #SFTX    - Flush the TX FIFO buffer.
RF_SWORRST          =0x3C    #SWORRST - Reset real time clock.
RF_SNOP             =0x3D    #SNOP    - No operation. Returns status byte.


beaconconfig=[
  IOCFG0,0x06,   #GDO0 Output Configuration
  FIFOTHR,0x47,  #RX FIFO and TX FIFO Thresholds
  PKTCTRL1, 0x04, #No address check.
  #PKTCTRL0, 0x05,#Packet Automation Control, variable length.
  PKTCTRL0, 0x04, #Packet automation control, fixed length.
  FSCTRL1,0x06,  #Frequency Synthesizer Control
  #FREQ2,0x21,    #Frequency Control Word, High Byte
  #FREQ1,0x62,    #Frequency Control Word, Middle Byte
  #FREQ0,0x76,    #Frequency Control Word, Low Byte
  MDMCFG4,0xF5,  #Modem Configuration
  MDMCFG3,0x83,  #Modem Configuration
  MDMCFG2,0x13,  #Modem Configuration
  DEVIATN,0x15,  #Modem Deviation Setting
  MCSM0,0x10,    #Main Radio Control State Machine Configuration
  FOCCFG,0x16,   #Frequency Offset Compensation Configuration
  WORCTRL,0xFB,  #Wake On Radio Control
  FSCAL3,0xE9,   #Frequency Synthesizer Calibration
  FSCAL2,0x2A,   #Frequency Synthesizer Calibration
  FSCAL1,0x00,   #Frequency Synthesizer Calibration
  FSCAL0,0x1F,   #Frequency Synthesizer Calibration
  TEST2,0x81,    #Various Test Settings
  TEST1,0x35,    #Various Test Settings
  TEST0,0x09,    #Various Test Settings
  ADDR,  0x00,   # ADDR      Device address.
  MCSM1, 0x30,   #MCSM1, return to IDLE after packet.  Or with 2 for TX carrier test.
  MCSM0,  0x18,  # MCSM0     Main Radio Control State Machine configuration.
  IOCFG2,  0x29, # IOCFG2    GDO2 output pin configuration.
  IOCFG0,  0x06, # IOCFG0    GDO0 output pin configuration.
  PKTLEN,  32,   # PKTLEN    Packet length.
  0,0  #Null terminator.
];


def packconfig(config):
    """Packs a radio configuration into a string."""
    strconfig="";
    for b in config:
        strconfig+=chr(b);
    return strconfig;
def stripnulls(msg):
    """Strips a strings to its first null terminator."""
    toret="";
    for b in msg:
        if b=='\0':
            return toret;
        toret+=b;
    return toret;

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
    def lcdstring(self,string):
        """Writes an 8-letter string to the LCD."""
        self.transact("\x03"+string+"\x00");
        return;
    def dmesg(self):
        """Returns the DMESG buffer."""
        return self.transact("\x04");
    def radioonoff(self,on=1):
        """Turns the radio on or off."""
        return self.transact("\x10"+chr(on));
    def radioconfig(self,configuration):
        """Configures the radio."""
        if type(configuration)==str:
            self.transact("\x11"+configuration);
        elif type(configuration)==list:
            self.transact("\x11"+packconfig(configuration));
    def radiofreq(self,freq):
        """Sets the radio frequency."""
        freqMult = (0x10000 / 1000000.0) / 26.0;
        num=int(freq*1e6*freqMult);
        freq2=(num>>16) & 0xFF;
        freq1=(num>> 8) & 0xFF;
        freq0= num      & 0xFF
        self.radioconfig([
            FREQ2, freq2,
            FREQ1, freq1,
            FREQ0, freq0,
            0, 0
        ]);

    def radiorx(self):
        """Sniffs for packets on the current channel."""
        return self.transact("\x12");
    def radiotx(self,message):
        """Sends a radio packet on the current frequency."""
        while(len(message)<32):
            message+='\x00';
        self.transact("\x13"+message);
        
if __name__=='__main__':
    parser = argparse.ArgumentParser(description='GoodWatch Client')
    parser.add_argument('-p','--port',
                        help='Serial Port',default='/dev/ttyUSB0');
    parser.add_argument('-r','--peek',
                        help='Peek');
    parser.add_argument('-l','--lcd',
                        help='Write a string the LCD.');
    parser.add_argument('-D','--dmesg',
                        help='Prints the dmesg.',action='count');

    parser.add_argument('-b','--beacon',
                        help='Transmits a beacon.');
    parser.add_argument('-B','--beaconsniff',
                        help='Sniffs for beacons.',action='count');
    
    args = parser.parse_args()

    goodwatch=GoodWatch(args.port);
    goodwatch.reset();
    goodwatch.setRST(False);

    #Switch to turbomode for more reliable comms.
    time.sleep(5);
    try:
        goodwatch.turbomode();
    except:
        print "turbo error."

    if args.peek!=None:
        adr=int(args.peek,16);
        val=goodwatch.peek(adr);
        print "0x%04x: %04x\n" % (adr,val);

    if args.lcd!=None:
        goodwatch.lcdstring(args.lcd);

    if args.dmesg>0:
        print goodwatch.dmesg();

    if args.beacon!=None:
        print "Turning radio on.";
        goodwatch.radioonoff(1);
        print "Configuring radio.";
        goodwatch.radioconfig(beaconconfig);
        goodwatch.radiofreq(433.0);
        while 1:
            print "Transmitting: %s" % args.beacon;
            goodwatch.radiotx(args.beacon+"\x00");
            time.sleep(1);
    
    if args.beaconsniff!=None:
        print "Turning radio on.";
        goodwatch.radioonoff(1);
        print "Configuring radio.";
        goodwatch.radioconfig(beaconconfig);
        goodwatch.radiofreq(433.0);
        while 1:
            packet=goodwatch.radiorx();
            #print packet.encode('hex');
            print stripnulls(packet);
            time.sleep(1);
            
        
    #Exit turbomode when we're done.
    #goodwatch.turbomode(0);
    
