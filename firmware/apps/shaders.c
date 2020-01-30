/*! \file shaders.c
  \brief Implements somfy RTS protocol
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


// TODO: optimize by doing it in place with an already 2*data_size array
// size is 14 and data to encode are starting at position size/2 (7)
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

static void make_raw_payload(uint8_t *data, uint32_t id, uint32_t rollingcode, uint8_t command){
  data[0] = SHADERS_ENC_KEY;
  data[1] = command << 4;
  data[2] = rollingcode >> 4;
  data[3] = rollingcode;
  data[4] = id >> 8;
  data[5] = id >> 4;
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
    
  //get the currently selected remote id
  //TODO
  int selected = 5; //TODO replace that

  uint32_t id = SHADERS_BASE_ID + selected;
  make_raw_payload(packet+SHADERS_PACKET_LENGTH-7, id, 12345, command);
  encrypt(packet+SHADERS_PACKET_LENGTH-7, 7);
  manchester_encode(packet, SHADERS_PACKET_LENGTH);
  packet_tx(packet, 14);
}

//! Set the rate.
//TODO: don't need that ?
// static void setrate(int index){
//   //First two bytes are the rate.
//   radio_writereg(MDMCFG4, ((uint8_t*) button_array[index])[0]);
//   radio_writereg(MDMCFG3, ((uint8_t*) button_array[index])[1]);
// }

//! Called after a transmission, or on a button press.
void shaders_packettx(){
  /* Schedule next packet if a number is being held.  We should send the same packet?
  */
  if(lastch<='9' && lastch>='0'){
    transmit(SHADERS_UP);
  }
}


//! Enter the OOK transmitter application.
void shaders_init(){
  /* Skip this application if we haven't got a radio.
   */
  if(!has_radio){
    app_next();
  }

  lcd_string(" SHADERS");

  //printf("%d button entries are available.\n", sizeof(button_array)/2);
}

//! Exit the OOK application.
int shaders_exit(){
  //Cut the radio off.
  radio_off();
  //Allow the exit.
  return 0;
}

//! Draw the OOK screen.
void shaders_draw(){
  int state=radio_getstate();

  switch(state){
  case 0:
  case 1:
    //lcd_string("     OOK");
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
  if( (lastch=ch) && ch>='0' && ch<='9' ){
    //Radio settings.
    radio_on();
    radio_writesettings(shaders_settings);
    radio_writepower(0x25);
    //Set a frequency manually rather than using the codeplug.
    radio_setfreq(433420000);

    //This handler will be called back as the packet finished transmission.
    shaders_packettx();
  }else{
    //Shut down the radio when the button is released.
    radio_off();
    lcd_zero(); //Clear the clock and radio indicators.
    lcd_string(" SHADERS");
  }
  return 0;
}
