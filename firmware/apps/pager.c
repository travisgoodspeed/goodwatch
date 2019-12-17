/*! \file pager.c
  \brief POCSAG Pager Receiver
  
  Quick little comm demo for receiving POCSAG pages, with an optional
  promiscuous mode.  Communications work great so long as the message
  fits into a single burst, as we don't yet have support for long
  packets.  Get an RIC divisible by 8 to be near the beginning of the
  burst.
  
  Relevant issues:
  #118 POCSAG/DAPNET Support.
  #107 Long Packet Support.  (Necessary for multi-burst packets.)
  #56  LCD charge pump is disabled during RX on CC430F6137.
  
  
  The power management situation for this applet is ugly, as we only
  have a CR2016 coin cell and replacing it requires disassembling the
  watch.  Our current solution is to hit the exit timer when a packet
  is received for any user, so that the watch will return to the clock
  when out of radio range.

  The radio is kept in idle mode (1mA) except when checking for the
  preamble every 250ms, when it comes to RX mode (16mA).  Total power
  consumption averages 4.25mA, allowing for twenty-four hours of
  active use.  Hopefully with finer timing, we can drop this down a
  bit.
*/

#include<stdio.h>
#include<string.h>
#include<msp430.h>
#include "api.h"

/* Settings for POCSAG pager reception, first prototyped in python.
 */
static const uint8_t pocsag_settings[]={
  MDMCFG4, 0xF5,      // Modem Configuration, wide BW
  //MDMCFG4, 0xC5,    // Modem Configuration, narrow BW
  MDMCFG3, 0x83,      // Modem Configuration
  MDMCFG2, 0x82,      // 2-FSK, current optimized, 16/16 sync
  MDMCFG1, 0x72,      // Long preamble.
  //FREND0 , 0x11,    // Front End TX Configuration

  //DEVIATN, 0x24,    // 9.5 kHz
  DEVIATN, 0x31,      // 15 kHz
  
  FSCAL3 , 0xE9,      // Frequency Synthesizer Calibration
  FSCAL2 , 0x2A,
  FSCAL1 , 0x00,
  FSCAL0 , 0x1F,
  
  PKTCTRL0, 0x00,     // Packet automation control, fixed length without CRC.
  PKTLEN,  64,        // PKTLEN    Packet length.
  
  SYNC1, 0x83,        //Sync word is 0x7CD215D8, but inverted from differing
  SYNC0, 0x2d,        //2FSK definitions, the first two bytes become 832d.
  ADDR,  0xea,        //EA27 is next, and we can at least match the first byte.
  
  TEST2,   0x81,    //Who knows?
  TEST1,   0x35,
  TEST0,   0x09,


  MCSM1,   0x30,  // MCSM1, return to IDLE after packet.
  //MCSM0,   0x10,  // MCSM0     Calibrate before RX or TX.
  //MCSM0,   0x30,  // MCSM0     Calibrate after every 4th packet.
  //MCSM0,   0x31,  // MCSM0     Calibrate after every 4th packet, keep osc on.
  //MCSM0,   0x3C,  // MCSM0
  MCSM0,   0x1C,  // MCSM0     Calibrate every packet.
  IOCFG2,  0x29,  // IOCFG2    GDO2 output pin configuration.
  IOCFG0,  0x06,  // IOCFG0    GDO0 output pin configuration.
  
  FIFOTHR,  0x47, //RX FIFO and TX FIFO Thresholds, TEST preserved on power down.
  //PKTCTRL1, 0x00, //No address check, no appended status.
  PKTCTRL1, 0x01, //Exact address check, no appended status.
  
  //Freq from Python
  FREQ2, 0x10,
  FREQ1, 0xEC,
  FREQ0, 0x30,
  
  0, 0
};

/* Correct addressing for POCSAG.
 */
static const uint8_t pocsag_settings_packet[]={
  PKTLEN,  64,        // PKTLEN    Packet length.
  
  SYNC1, 0x83,        //Sync word is 0x7CD215D8, but inverted from differing
  SYNC0, 0x2d,        //2FSK definitions, the first two bytes become 832d.
  ADDR,  0xea,        //EA27 is next, and we can at least match the first byte.
  
  PKTCTRL1, 0x01, //Exact address check, no appended status.
  
  0, 0
};

/* Settings to match on the preamble, for waking up.
 */
static const uint8_t pocsag_settings_preamble[]={
  PKTLEN,  1,        // PKTLEN    Packet length of one, so that we match as soon as possible.
  
  SYNC1, 0xAA,       // Triggers an early match if the preamble is heard.
  SYNC0, 0xAA,
  ADDR,  0xAA,

  PKTCTRL1, 0x00, //No address check, no appended status.
  
  0, 0
};

static uint16_t wakecount=0;
static char lastpacket[]="IDLE      ";

//! Handle an incoming packet.
void pager_packetrx(uint8_t *packet, int len){
  /* When the packet arrives, we need to chunk it into the pocsag
     library. */
  int i;
  uint32_t *words;

  /* The power management of the watch will have it exit to the main
    (low-power) screen every three minutes unless app_cleartimer() is
    called.  The pager applet calls this function only when a packet
    arrives, so that the watch will stop burning its battery if it
    leaves the range of the DAPNET transmitter.

    Thanks to DAPNET subscriber 2007679 for having enough automated
    traffic to make this scheme work!
    
   */
  app_cleartimer();
  
  
  
  /* When the first byte is AA, it's because we've matched on the
     preamble.  This indicates that a packet is coming within the next
     480ms, and we ought to stay awake for it.
   */
  if(packet[0]==0xaa){
    //printf("Found the preamble!\n");
    
    //Keep the radio on for one second.
    wakecount=4;

    //Start looking for the real packet.
    radio_writesettings(pocsag_settings);
    radio_writesettings(pocsag_settings_packet);
    packet_rxon();

    return;
  }

  //printf("Found the packet!\n");
  
  
  /* See pocsag.c for decoder info, but the jist is that the first two
     bytes are the last two bytes of the FCS, and following words come
     in 4-byte words.  The pocsag library expects these words to have
     already been bitflipped (^=0xFFFFFFFF) and loaded as 32-bit *BIG
     ENDIAN* words.  (The MSP430 is little endian.  Sorry.)

     /FCS\ /--word0--\ /--word1--\ /--word2--\ ..
     ea 27 ff d8 da c8 3a ee f9 6c 7e 66 3a 50 ..
  */


  /* Forgive this pointer arithmetic, but in my own head, it really
     makes more sense this way.  First we skip the first two bytes (ea
     27) to get an array of the packet words.
   */
  words=(uint32_t*) (packet+2);
  
  /* We've triggered at the beginning of the batch, so we inform the
     pocsag library of that and then have it handle each word in
     sequence.  __builtin_bswap32() is a GCC primitive to swap a
     32-bit word, much like htonl() would do.
  */
  pocsag_newbatch();
  for(i=0;i<16;i++){
    pocsag_handleword(__builtin_bswap32(words[i])^0xFFFFFFFF);

    
    //Display the packet if it newly matches our ID, or if we have no ID.
    if(pocsag_lastid==DAPNETRIC || DAPNETRIC==0){
      memcpy(lastpacket, pocsag_buffer, 8);
      printf("Displaying: '%s'\n", lastpacket);
    }
  }

  //Zero the packet just so bugs are clear.
  memset(packet,0xFF,len);
  printf("%ld: %s\n\n",
         pocsag_lastid, pocsag_buffer
         );
  
}



//! Enter the Pager application.
void pager_init(){
  /* This enters the pager application, confiures the radio to the
     right frequency, and then enters RX mode.
   */
  if(has_radio){
    printf("Tuning the pager to Dapnet.\n");
    radio_on();

    //Settings are self-contained.
    radio_writesettings(pocsag_settings);
    radio_writepower(0x25);
    
    radio_setfreq(439988000);
    //packet_rxon();
  }else{
    app_next();
  }
}

//! Exit the Pager application.
int pager_exit(){
  //Stop listening for packets.
  packet_rxoff();
  //Cut the radio off.
  radio_off();
  //Allow the exit.
  return 0;
}

//! Draw the Pager screen.
void pager_draw(){

  /* So if it weren't for the power budget and the LCD charge pump
     bug, we'd just leave the radio receiving all the time.  But that
     would kill our coincell in six hours!
     
     Instead we make use of the drawing routine, which is triggered
     every 250ms, to look for a short fragment of POCSAG's 480ms long
     preamble.  We don't need to look for very long, as the preamble
     ought to already be on the air during our spot check, and if it
     isn't found, we can quickly fall back to sleep until the next
     frame.
     
   */
  
  
  int state=radio_getstate();
  int i=0;


  if(state==0 || state==1){
    /* We are idling or off, so no packet is known to be inbound, and we
       should wake up to look for a preamble.  If it doesn't come by
       the end of this function, we'll give up and sleep for a while.
     */

    //Start looking for the preamble.
    //radio_on();
    radio_writesettings(pocsag_settings);
    radio_writesettings(pocsag_settings_preamble);
    packet_rxon();

    //Wait until we're in RX mode.
    while(radio_getstate()!=13);

    //Then wait long enough for the packet.
    /* 100 works
       75 works
       60 seems to work reliably, 3.9mA or 25h
       55 loses some packets, 3.7mA or 26h
       50 can be tempermental, 3.6mA or 27h
     */
    for(i=0;i<60;i++)
      __delay_cycles(1000);
  }
  
  
  state=radio_getstate();
  
  if(state==1 || state==13){
    /* Draw the last incoming packet on the screen. */
    lcd_zero();
    lcd_string(lastpacket);
  }else{
    printf("Unexpected state %d\n", state);
    lcd_number(state);
  }

  switch(state){
  case 0: //OFF
  case 1: //IDLE
    /* The idle state still draws 1mA, so we would prefer to shut down
       the receiver entirely if that's possible.  For now, we'll just
       be grateful that we aren't drawing the full 16mA of RX mode.
     */
    break;
  case 13: //RX Mode
    /* We only remain in RX mode if we've seen the preamble and know
       that a packet is coming.  Even then, we give up after a second,
       because battery life is precious.
     */
    if(wakecount){
      wakecount--;
    }else{
      packet_rxoff();
      //radio_off();
    }
    break;
  default:
    printf("state=%d\n",
           state);
  }

}

//! Keypress handler for the pager applet.
int pager_keypress(char ch){
  /* For now, the pager doesn't bother responding to keypresses.  In
     the future, it might be nice for this to show (1) the timestamp
     of the last received packet and (2) a promiscuous mode showing
     all recent packets.
   */

  return 0;
}

