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
  watch.  Our current solution is to only hit the exit timer when a
  packet is received for any users, so that the watch will return to
  the clock when out of radio range.  A better solution will be to
  match the timeslot of the transmitter, so that we can safely sleep
  during the interrum between transmissions.
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


  
  MCSM1,   0x30,  // MCSM1, return to IDLE after packet.  Or with 2 for TX carrier tes.
  MCSM0,   0x10,  // MCSM0     Main Radio Control State Machine configuration.
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
  printf("\n%ld: %s\n",
         pocsag_lastid, pocsag_buffer
         );
  
  

  packet_rxon();
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
    packet_rxon();
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
  int state=radio_getstate();
  
  if(state==1 || state==13){
    /* Draw the last incoming packet on the screen. */
    lcd_string(lastpacket);
  }else{
    lcd_zero();
    lcd_number(state);
  }
  
  switch(state){
  case 1: //IDLE
    packet_rxon();
    break;
  case 22: //TX_OVERFLOW
    printf("TX Overflow.\n");
    radio_strobe(RF_SIDLE);
    break;
    
  case 13: //RX Mode
    /* The screen will dim down in this mode on a CC430F6137 unless we
       power cycle it, but that's not this module's responsibility.
       See issue #56 on Github or use a CC430F6147.
    */
    break;
  }

}

//! Keypress handler for the pager applet.
int pager_keypress(char ch){
  /* For now, the pager doesn't bother responding to keypresses.  In
     the future, it might be nice for this to show (1) the timestamp
     of the last received packet and (2) a promiscuous mode showing
     all recent packets.
     
     We shouldn't go too far, however, because of the limited battery
     life.
   */
  return 0;
}
