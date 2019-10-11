/* 
 * File:   jukebox.c
 * Author: NotPike
 */

#ifdef STANDALONE

#include<stdio.h>
#include<stdint.h>
#include<string.h>
#include "jukebox.h"

/*=========================== V A R I A B L E S ===========================*/
#define LEN 16 //Bytes
static char lastch = 0;

// Commands
const uint32_t commands[32] = {
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
		if (decodeMsg & 0x80000000UL) { // If 1
			if (bitSize <= 4) {
				encodeMsg <<= 4;
				encodeMsg |= 0x8; // 1000 
				decodeMsg <<= 1;
				bitSize += 4;
			} else if (bitSize == 6) { // Split the Byte
				encodeMsg <<= 2;
				encodeMsg |= 0x2;
				out[arrayPos] = encodeMsg; // Add Byte to char array
				arrayPos++;
				encodeMsg <<= 8; // Wipe the stack 
				decodeMsg <<= 1;
				bitSize = 2;
			} else if (bitSize == 8) {
				out[arrayPos] = encodeMsg; // Add Byte to char array
				arrayPos++;
				encodeMsg <<= 8; // Wipe the stack 
				encodeMsg |= 0x8; // 1000 
				decodeMsg <<= 1;
				bitSize = 4;
			}
		} else { // Else 0
			if (bitSize <= 6) {
				encodeMsg <<= 2;
				encodeMsg |= 0x2; // 10
				decodeMsg <<= 1;
				bitSize += 2;
			} else if (bitSize == 8) {
				out[arrayPos] = encodeMsg; // Add Byte to char array
				arrayPos++;
				encodeMsg <<= 8; // Wipe the stack 
				encodeMsg |= 0x2; // 10
				decodeMsg <<= 1;
				bitSize = 2;
			}
		}
	}

	// Add Tail
	if (bitSize == 2) {
		encodeMsg <<= 4;
		encodeMsg |= 0x8; // 1000
		encodeMsg <<= 2;
	} else if (bitSize == 4) {
		encodeMsg <<= 4;
		encodeMsg |= 0x8; // 1000 
	} else if (bitSize == 6) { // Split the Byte
		encodeMsg <<= 2;
		encodeMsg |= 0x2; // 10
		out[arrayPos] = encodeMsg; // Add Byte to char array
		arrayPos++;
		encodeMsg <<= 8; // Wipe the stack 
	} else if (bitSize == 8) {
		out[arrayPos] = encodeMsg; // Add Byte to char array
		arrayPos++;
		encodeMsg <<= 8; // Wipe the stack 
	}

	// Pad with 0x00 if msg length is not 16 Bytes
	if (arrayPos == 14) {
		out[14] = encodeMsg;
		out[15] = 0x00;
	} else {
		out[15] = 0x00;
	}
}


int main() {
	// Variables
	uint8_t returnValue[16];
	
	// Known Values
	uint8_t msg[16] = {0xFF, 0xFF, 0x00, 0xa2, 0x88, 0x8a, 0x2a, 0xaa, // Skip PIN:000
		           0xa2, 0x2a, 0x28, 0xaa, 0x22, 0x8a, 0x20, 0x00};
	uint8_t msg2[16] = {0xFF, 0xFF, 0x00, 0xa2, 0x88, 0x8a, 0x2a, 0xaa, // Edit Queue PIN:000
			   0xa8, 0x8a, 0xaa, 0x2a, 0x22, 0x22, 0x20, 0x00};
	
	
	// TEST 1
	printf("===Test 1 Skip for PIN 000===\n");
	encode(returnValue, commands[4], 0);

	// Step each char and cmp to msg
	int i;
	for(i =0; i < 16; i++) {
		if(msg[i] == returnValue[i]) {
			//printf("Byte %i: %u pass\n",i,returnValue[i]);
		} else {
			printf("Byte %i: %u fail\n",i,returnValue[i]);
			return(0);
		} 
	}
	printf("PASS!\n");

	// TEST 2
	printf("\n===Test 2 Edit Que for PIN 000===\n");
	encode(returnValue, commands[3], 0);

	// Step each char and cmp to msg2
	for(i =0; i < 16; i++) {
		if(msg2[i] == returnValue[i]) {
			//printf("Byte %i: %u pass\n",i,returnValue[i]);
		} else {
			printf("Byte %i: %u fail\n",i,returnValue[i]);
			return(0);
		} 
	}
	printf("PASS!\n");

	// TEST 3
	// Encode all 32 Commands and 256 PINs
	printf("\n==Test 3 Encode All 32 Commands and 256 PINs==\n"); 
	for(i=0; i < 255; i++) {
		int j;
		for(j = 0; j < 32; j +=1) {
			encode(returnValue, commands[j], i);
		}
	}
	printf("Tests Complete!\n");

	return (0);
}

#endif

