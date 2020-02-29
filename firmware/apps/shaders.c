/*! \file shaders.c
  \brief Implements somfy RTS protocol

  This is an implementation of the somfy RTS protocol to control
  window blinds using the goodwatch.
  
  With the RTS protocol, each remote has a unique id and a rolling code 
  associated. We a button is pressed, a packet is broadcasted and the 
  window blinds around are triggered depending on if the id is registered as
  one of their controler(s) and if the rolling code matches.

  This app lets you register your goodwatch as a new remote for your blind.
  It will have its own rolling code and so will not desynchronize your real
  remote. To register and use a remote:
   - put the blind is learn mode by holding the red button at the back of an
     already adopted remote. The shader will go up and down to mean it's in 
     learn mode.
   - on the watch, press for instance '3' to select id '3' and then press '=' to
     send the 'register' packet. The shaders will go up and down. From now on,
     the shader will accept packets from your watch from your id '3'.
   - you can now press '/' to go up, '*' for the 'MY' button, and '-' to go down.
     If you want to change id (let's say 5), press 5 and then the command you want.
     Rolling codes are updated per id.

  Since the protocol contains rolling codes, checksums and is obfuscated, the 
  packets are recomputed for every keypress.
  The packet is made of a prefix (wake up + preamble) to with manchester encoded 
  data is appended. The data itself is xor obfuscated. Take a look at 
  https://pushstack.wordpress.com/somfy-rts-protocol/ for more information.
  
*/

#include <stdio.h>
#include <string.h>
#include <msp430.h>
#include "api.h"
#include "shaders.h"


static const uint8_t shaders_settings[]={
  //Change these to change the rate.
  MDMCFG4, 0xF5,      // Modem Configuration
  MDMCFG3, 0xF6,      // Modem Configuration
  //These rest are consistent for all OOK emulation.
  MDMCFG2, 0x30,      // Modem Configuration, no sync
  FREND0 , 0x11,      // Front End TX Configuration
  FSCAL3 , 0xE9,      // Frequency Synthesizer Calibration
  FSCAL2 , 0x2A,      // Frequency Synthesizer Calibration
  FSCAL1 , 0x00,      // Frequency Synthesizer Calibration
  FSCAL0 , 0x1F,      // Frequency Synthesizer Calibration
  PKTCTRL0, 0x00,     // Packet automation control, fixed length without CRC.
  PKTLEN,  SHADERS_PACKET_LENGTH,  // PKTLEN    Packet length.
  0,0
};

//! All rolling codes are initialized to 0 at boot.
static uint16_t rolling_codes[9] = {0,0,0,0,0,0,0,0,0};

//prefix to add before encoded data
static const uint8_t prefix[SHADERS_PREFIX_LENGTH] = "\xff\xff"  
                                    //pad with zeros
                                    "\x00\x00\x00\x00"
                                    "\x00\x00\x00\x00"
                                    "\x00\x00\x00\x00"
                                    //preamble
                                    "\x01\xe1\xe1\xfe";

static uint32_t selected_id = 1;

// size should be 14 and data to encode are starting at position size/2 (7)
static void manchester_encode(uint8_t *data, size_t size){
  unsigned int starting_pos = size/2;
  unsigned int i;
  unsigned int j;
  uint8_t v;
  uint8_t bit;
  for(i=starting_pos ; i<size ; i++){
    //vh
    v = 0;
    for(j=4 ; j<8 ; j++){
      bit = ((data[i] << (7-j)) & 255) >> 7;
      v += (2 ^ (bit << 1 | bit)) << (2*(j-4));
    }
    //vl
    data[2*(i-starting_pos)] = v;
    v = 0;
    for(j=0 ; j<4 ; j++){
      bit = ((data[i] << (7-j)) & 255) >> 7;
      v += (2 ^ (bit << 1 | bit)) << (2*j);
    }
    data[2*(i-starting_pos)+1] = v;
  }
}

// encrypts in place
static void encrypt(uint8_t *data, size_t size){
  int i;
  for(i=1 ; i<size ; i++){
    data[i] = data[i-1] ^ data[i];
  }
}

//computes and insert the crc
static void set_crc(uint8_t *data){
  uint8_t crc = 0;
  int i;
  //computes CRC for polynomial x^4+1
  for (i=0 ; i< SHADERS_RAW_PAYLOAD_LENGTH ; i++){
    crc = crc ^ data[i] ^ (data[i] >> 4);
  }
  data[1] = data[1] | (crc & 0x0f);
}

static void make_raw_payload(uint8_t *data, uint32_t id, uint8_t command){
  data[0] = SHADERS_ENC_KEY;
  data[1] = command << 4;
  data[2] = rolling_codes[selected_id-1] >> 8;
  data[3] = rolling_codes[selected_id-1];
  data[4] = id >> 16;
  data[5] = id >> 8;
  data[6] = id;

  set_crc(data);
}

//! Handle an incoming packet.
void shaders_packetrx(uint8_t *packet, int len){
  printf("Not yet supported.\n");
}

static char lastch=0;

//! Send a packet.
static void transmit(int command){
  uint8_t packet[SHADERS_PACKET_LENGTH];
  memset(packet, 0, SHADERS_PACKET_LENGTH);

  uint32_t id = SHADERS_BASE_ID + selected_id;
  make_raw_payload(packet+SHADERS_PACKET_LENGTH-SHADERS_RAW_PAYLOAD_LENGTH, id, command);
  encrypt(packet+SHADERS_PACKET_LENGTH-SHADERS_RAW_PAYLOAD_LENGTH, SHADERS_RAW_PAYLOAD_LENGTH);
  manchester_encode(packet+SHADERS_PACKET_LENGTH-2*SHADERS_RAW_PAYLOAD_LENGTH, 2*SHADERS_RAW_PAYLOAD_LENGTH);
  memcpy(packet, prefix, SHADERS_PREFIX_LENGTH);
  packet_tx(packet, SHADERS_PACKET_LENGTH);
}

//! Called after a transmission, or on a button press.
void shaders_packettx(){
  /* Schedule next packet if a number is being held.  We should send the same packet?
  */
  switch (lastch)
  {
  case '/':
    transmit(SHADERS_UP);
    break;
  case '*':
    transmit(SHADERS_MY);
    break;
  case '-':
    transmit(SHADERS_DOWN);
    break;
  case '=':
    transmit(SHADERS_PROG);
  }
}


//! Enter the shaders transmitter application.
void shaders_init(){
  /* Skip this application if we haven't got a radio.
   */
  if(!has_radio){
    app_next();
  }

  lcd_string(" SHADERS");
}

//! Exit the shaders application.
int shaders_exit(){
  //Cut the radio off.
  radio_off();
  //Allow the exit.
  return 0;
}

//! Draw the shaders screen.
void shaders_draw(){
  int state=radio_getstate();

  switch(state){
  case 0:
  case 1:
    break;
  case 19: //RX IDLE between transmit packets.
    lcd_string("TRANSMIT");
    break;
  case 22: //TX_OVERFLOW
    printf("TX Overflow.\n");
    radio_strobe(RF_SIDLE);
    break;
  default:
    lcd_hex(state);
    break;
  }
}

//! Keypress handler for the shaders applet.
int shaders_keypress(char ch){
  /* When a key is first pressed, we call shaders_packettx() to kick off a
     transmission.  That function is also the callback from the packet
     library, so it will keep running in a loop until the key is
     released.
   */ 
  if(ch<='9' && ch>='1'){
    selected_id = ch-'0';
    return 0;
  }

  if(ch=='/' || ch=='*' || ch=='-' || ch=='='){
    lastch = ch;
    //Radio settings.
    radio_on();
    radio_writesettings(shaders_settings);
    radio_writepower(0x25);
    //Set a frequency manually rather than using the codeplug.
    radio_setfreq(433420000);

    //This handler will be called back as the packet finished transmission.
    shaders_packettx();
  }else{
    if (lastch=='/' || lastch=='*' || lastch=='-' || lastch=='='){
      //Increase rolling code when the button is release
      rolling_codes[selected_id-1] = rolling_codes[selected_id-1]+1;
    }
    lastch = ch;
    //Shut down the radio when the button is released.
    radio_off();
    lcd_zero(); //Clear the clock and radio indicators.
    lcd_string(" SHADERS");
  }
  return 0;
}
