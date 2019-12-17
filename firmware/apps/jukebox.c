/*   _____ _            _____               
 *  |_   _| |__   ___  |  ___|__  _ __  ____
 *    | | | '_ \ / _ \ | |_ / _ \| '_ \|_  /
 *    | | | | | |  __/ |  _| (_) | | | |/ / 
 *    |_| |_| |_|\___| |_|  \___/|_| |_/___| GoodWatch Edition
 * 
 * Author: NotPike
 * Twitter: @IfNotPike
 * License: MIT
 * 
 *  ====Quick Start====
 *  ____________________
 * / ,---------------.  \
 * | |    JUKEBOX    |  |
 * | `---------------'  |
 * | [7]  [8]  [9]  [%] |
 * | power ^  editQ     |
 * | [4]  [5]  [6]  [X] |
 * | <--  O K  -->      |
 * | [1]  [2]  [3]  [-] |
 * | pause v  saveQ     |
 * | [0]  [.]  [=]  [+] |
 * \_skip_______________/
 * 
 * 
 * ==TouchTunes Info==
 * Frequency: 433.92MHz
 * Modulation: ASK/OOK
 * Protocol: NEC
 * Symbol Rate: 1766
 * Symbol Period: 566us
 * 
 * ==NEC Format==
 * Short(0): 10   (ON OFF)
 * Long (1): 1000 (ON OFF OFF OFF)
 * 
 * Structure:                 <SYNC> <PREAMBLE> <PIN> <COMMAND>
 * Sync (Literal Symbols):    \xFF\xFF\x00
 * Preamble (Decoded NEC):    0x5D
 * PIN 000-255 (Decoded NEC): 0x00-0xFF (LSB)
 * Command (Decoded NEC):
 * 
 * Note: Commands are doubled with the 2nd half being reversed.
 *       For example, Pause 0x32 will translate to 0x3223 before
 *       being encoded to the literal symbols. \xA8\x8A\x8A\xA2\xA8\x80
 * 
 * Command:             Pin 000 - On/Off
 * Literal Symbols HEX: ffff00 a2888a2 aaaa 8888aa2aa2220
 * Literal Symbols BIN: 11111111111111110000 10 1000 10 1000 1000 1000 10 1000 10 10 10 10 10 10 10 10 10 1000 1000 1000 1000 10 10 10 1000 10 10 10 10 1000 1000 1000 100000
 * Decoded Symbols:             SYNC         S  L    S  L    L    L    S  L    S  S  S  S  S  S  S  S  S  L    L    L    L    S  S  S  L    S  S  S  S  L    L    L    TAIL
 * 
 * ==More Info==
 * https://github.com/notpike/The-Fonz
 * https://github.com/furrtek/portapack-havoc/blob/master/firmware/application/apps/ui_touchtunes.hpp
 * 0w0
 */

#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <msp430.h>
#include "api.h"

/*========================== P R O T O T Y P E S ==========================*/
void jukebox_packettx();

/*=========================== V A R I A B L E S ===========================*/
#define LEN 17 //Bytes
static char lastch = 0;

// PIN
char pinChar[8] = "PIN     ";
int pinFlag = 1;
int pin = 0;

// Commands
const uint32_t jukebox_commands[32] = {
	0x32, // Pause
	0x78, // On/Off
	0x70, // P1 
	0x60, // P2 Edit Queue
	0xCA, // P3 Skip 
	0x20, // F1 Restart
	0xF2, // Up 
	0xA0, // F2 Key
	0x84, // Left
	0x44, // OK
	0xC4, // Right
	0x30, // F3 Mic A Mute
	0x80, // Down 
	0xB0, // F4 Mic B Mute
	0xF0, // 1 
	0x08, // 2
	0x88, // 3
	0x48, // 4
	0xC8, // 5
	0x28, // 6
	0xA8, // 7
	0x68, // 8
	0xE8, // 9
	0x18, // * Music_Karaoke
	0x98, // 0
	0x58, // # Lock_Queue
	0xD0, // Zone 1 Vol+
	0x90, // Zone 2 Vol+
	0xC0, // Zone 3 Vol+
	0x50, // Zone 1 Vol-
	0x10, // Zone 2 Vol-
	0x40, // Zone 3 Vol-
};

// MSP430 Config
static const uint8_t jukebox_settings[] = {
	MDMCFG4, 0xF6, // Modem Configuration
	MDMCFG3, 0x1D, // Modem Configuration
	MDMCFG2, 0x30, // Modem Configuration, no sync
	FREND0, 0x11, // Front End TX Configuration
	FSCAL3, 0xE9, // Frequency Synthesizer Calibration
	FSCAL2, 0x2A, // Frequency Synthesizer Calibration
	FSCAL1, 0x00, // Frequency Synthesizer Calibration
	FSCAL0, 0x1F, // Frequency Synthesizer Calibration
	PKTCTRL0, 0x00, // Packet automation control, fixed length without CRC.
	PKTLEN, LEN, // PKTLEN    Packet length.
	0, 0
};

/*================================ M A I N ================================*/

// Start
void jukebox_init() {
	if (!has_radio) {
		app_next();
	}
	printf("10 button entries are available for Jukebox.\n");
	lcd_string(pinChar); // Draw screen
}

// Exit
int jukebox_exit() {
	pinChar[5] = ' ';
	pinChar[6] = ' ';
	pinChar[7] = ' ';
	pinFlag = 1;
	pin = 0;
	radio_off();
	return 0;
}

/*================================= T X =================================*/

// NEC Encoder 
// Logic refered from Havoc Portapack Firmware
void encode(uint8_t *out, uint32_t command, int pin) {

	// Variables
	uint8_t bit; // Bit counter
	uint32_t decodeMsg = 0x5D; // Sync Word + decoded value
	uint8_t encodeMsg = 0; // Bit holder before encoded values are moved to *out		      

	// Preamble 0xFFFF00
	out[0] = 0xFF;
	out[1] = 0xFF;
	out[2] = 0x00;

	// PIN, LSB First
	for (bit = 0; bit < 8; bit++) {
		decodeMsg <<= 1; // Shift left 1 bit
		if (pin & (1 << bit)) { // If both bits line up add 1
			decodeMsg |= 1;
		}
	}

	// Command and it's complement
	decodeMsg <<= 16; // Shift left 16 bits
	decodeMsg |= (command << 8); // Add command shift left 8 bits
	decodeMsg |= (command ^ 0xff); // Add command's complement

	// NEC Encode
	uint8_t bitSize = 0; // Size counter for encodeMsg
	int arrayPos = 3; // Pointer counter for *out
	for (bit = 0; bit < (8 + 8 + 16); bit++) { // Sync + Pin + Command == 32bit
		if (decodeMsg & 0x80000000UL) {		   // If 1
			if (bitSize <= 4) {
				encodeMsg <<= 4;
				encodeMsg |= 0x8;			   // 1000 
				decodeMsg <<= 1;
				bitSize += 4;
			} else if (bitSize == 6) {		   // Split the Byte
				encodeMsg <<= 2;
				encodeMsg |= 0x2;
				out[arrayPos] = encodeMsg;	   // Add Byte to char array
				arrayPos++;
				encodeMsg <<= 8;			   // Wipe the stack 
				decodeMsg <<= 1;
				bitSize = 2;
			} else if (bitSize == 8) {
				out[arrayPos] = encodeMsg;	  // Add Byte to char array
				arrayPos++;
				encodeMsg <<= 8;			  // Wipe the stack 
				encodeMsg |= 0x8;			  // 1000 
				decodeMsg <<= 1;
				bitSize = 4;
			}
		} else { // Else 0
			if (bitSize <= 6) {
				encodeMsg <<= 2;
				encodeMsg |= 0x2;			  // 10
				decodeMsg <<= 1;
				bitSize += 2;
			} else if (bitSize == 8) {	
				out[arrayPos] = encodeMsg;	  // Add Byte to char array
				arrayPos++;
				encodeMsg <<= 8;			  // Wipe the stack 
				encodeMsg |= 0x2;			  // 10
				decodeMsg <<= 1;
				bitSize = 2;
			}
		}
	}

	// Add Tail
	if (bitSize == 2) {
		encodeMsg <<= 4;
		encodeMsg |= 0x8;					 // 1000
		encodeMsg <<= 2;
		out[arrayPos++] = encodeMsg;		 // Add Byte to char array

	} else if (bitSize == 4) {
		encodeMsg <<= 4;
		encodeMsg |= 0x8;					 // 1000 
		out[arrayPos++] = encodeMsg;		 // Add Byte to char array

	} else if (bitSize == 6) {				 // Split the Byte
		encodeMsg <<= 2;
		encodeMsg |= 0x2;					 // 10
		out[arrayPos++] = encodeMsg;		 // Add Byte to char array
		encodeMsg <<= 8;					 // Wipe the stack
		
	} else if (bitSize == 8) {
		out[arrayPos++] = encodeMsg;		 // Add Byte to char array
		encodeMsg <<= 4;					 // Wipe the stack
		encodeMsg |= 0x8;					 // 1000 
		encodeMsg <<= 4;					 // Wipe the stack		
		out[arrayPos++] = encodeMsg;		 // Add Byte to char array	
	}

	// Pad with 0x00 if msg length is not 18 Bytes
	while(arrayPos < LEN){
		out[arrayPos++] = 0x00;		
	}
}

// Build Packet Helper Function
uint8_t* build_jukebox_packet(int cmd, int pin) {
	static int lastpin = -1, lastcmd = -1;
	static uint8_t packet[LEN]; //Must be static so it isn't overwritten.

	//Update the packet only if the pin has changed.
	if (lastpin != pin || lastcmd != cmd) {
		encode(packet, jukebox_commands[cmd], pin);
	}
	//Return our static packet value.
	return packet;
}

/*================================= R X =================================*/

/* The plan is have the '+' act is the UI for the user to switch between
 * RX and TX mode. While in RX mode the radio will tune to 433.92MHz and
 * decode any messages being transmitted. The PIN of the message will then
 * be displayed on the LCD. No PIN will be displayed as '***' to prevent 
 * confusion. Last known PIN will be displayed unless it changes or the user
 * switches back to TX mode.   
 */

void jukebox_packetrx(uint8_t *packet, int len) {
	printf("Not yet supported.\n");
}

/*================================= U I =================================*/

// PIN Input
void pinInput() {
	if (pinChar[5] == ' ') {
		printf("char: %c\n", lastch);
		pinChar[5] = lastch;
		pin = (lastch - '0') * 100;
	} else if (pinChar[6] == ' ') {
		pinChar[6] = lastch;
		pin += (lastch - '0') * 10;
	} else if (pinChar[7] == ' ') {
		pinChar[7] = lastch;
		pin += (lastch - '0');

		if (pin <= 255) { // User can't input a number greater then 255
			pinFlag = 0;
		} else { // User goofed, try again
			pinChar[5] = ' ';
			pinChar[6] = ' ';
			pinChar[7] = ' ';
		}
	}
	lcd_string(pinChar); // Update screen
}

// Keypress Handler
int jukebox_keypress(char ch) {
	if (pinFlag && (lastch = ch) && ch >= '0' && ch <= '9') {
		pinInput();
	} else if (!pinFlag && (lastch = ch) && ch >= '0' && ch <= '9') {
		// Radio Settings
		radio_on();
		radio_writesettings(jukebox_settings);
		radio_writepower(0xB0);
		radio_setfreq(433920000); // 433.92MHz

		//This handler will be called back as the packet finished transmission.
		jukebox_packettx();
	} else if (!pinFlag) {
		//Shut down the radio when the button is released.
		radio_off();
		lcd_zero(); //Clear the clock and radio indicators.
		lcd_string("JBOX  TX");
	}
	return 0;
}

// Button Mapping

void jukebox_packettx() {
	if (lastch <= '9' && lastch >= '0') {
		switch (lastch - '0') {
			case 0: // Skip
				packet_tx(build_jukebox_packet(4, pin), LEN);
				lcd_string("  Skip  ");
				break;
			case 1: // Pause
				packet_tx(build_jukebox_packet(0, pin), LEN);
				lcd_string(" Pause  ");
				break;
			case 2: // Down Arrow
				packet_tx(build_jukebox_packet(12, pin), LEN);
				lcd_string("D Arrow ");
				break;
			case 3: // Lock Queue
				packet_tx(build_jukebox_packet(25, pin), LEN);
				lcd_string(" Lock Q ");
				break;
			case 4: // Left Arrow
				packet_tx(build_jukebox_packet(8, pin), LEN);
				lcd_string("L Arrow ");
				break;
			case 5: // OK
				packet_tx(build_jukebox_packet(9, pin), LEN);
				lcd_string("   OK   ");
				break;
			case 6: // Right Arrow
				packet_tx(build_jukebox_packet(10, pin), LEN);
				lcd_string("R Arrow ");
				break;
			case 7: // Power
				packet_tx(build_jukebox_packet(1, pin), LEN);
				lcd_string(" Power  ");
				break;
			case 8: // Up Arrow
				packet_tx(build_jukebox_packet(6, pin), LEN);
				lcd_string("Up Arrow");
				break;
			case 9: // Edit Queue
				packet_tx(build_jukebox_packet(3, pin), LEN);
				lcd_string(" Edit Q ");
				break;
		}
	}
}

// Draw Screen
void jukebox_draw() {
	int state = radio_getstate();

	switch (state) {
		case 0: //Skip
		case 1:
			break;
		case 19: //RX IDLE between transmit packets.
			break;
		case 22: // TX OVERFLOW
			printf("TX Overflow.\n");
			radio_strobe(RF_SIDLE);
			break;
		default:
			lcd_hex(state);
			break;
	}
}
