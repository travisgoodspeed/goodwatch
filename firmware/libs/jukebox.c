/* 
 * File:   jukebox.c
 * Author: NotPike
 */

#ifdef STANDALONE

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <inttypes.h>
#include <assert.h>
#include "jukebox.h"

/*=========================== V A R I A B L E S ===========================*/
#define LEN 17 //Bytes
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
		} else {							  // Else 0
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
		//encodeMsg <<= 8;					 // Wipe the stack
		
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


/*================================= R X =================================*/
void msgConcat(const uint8_t *in, uint8_t *inPos, uint64_t *encode, uint8_t *bitLength) {

	while(*bitLength <= 24 && *inPos < LEN) {
		*encode <<= 8;							// Shift left 8
		*encode |= in[*inPos];					// Add RX data and increment by 1
		*inPos += 1;
		*bitLength += 8;						// Update bitLength
	}
}

// NEC Decoder
// *out[PIN, Command Value]
int decode(const uint8_t *in, uint8_t *out) {
	
	uint64_t encode = 0;
	uint64_t decode = 0;
	uint8_t bitLength = 0;
	uint8_t inPos = 3;
	uint8_t pin = 0;
	uint8_t command = 0x00;
	
	// If the packet is valid
	if(in[0] == 0xff && in[1] == 0xff && in[2] == 0x00) {
		
		// Makes sure decode does not overflow
		msgConcat(in, &inPos, &encode, &bitLength);
		
		// Decode
		while(bitLength) {
			decode <<= 1;											// Shift left 1, if 0, stays 0
			if(encode >> ((uint64_t)bitLength - 2) == 2 &&			// If 10 then 0
			   encode >> ((uint64_t)bitLength - 4) != 8 &&			// But not 1000
			   encode >> ((uint64_t)bitLength - 4) != 0	) {			// Or 0000...
				//printf("0\n");
				encode &= ~(1 << ((uint64_t)bitLength - 1));		// Turns off left bit
				bitLength -= 2;										// Update bitLength
			} else if(encode >> ((uint64_t)bitLength - 4) == 8) {	// if 1000 then 1
				//printf("1\n");
				encode &= ~(1 << ((uint64_t)bitLength - 1));		// Turns off left bit
				decode |= 1;										// Adds a 1 to decode
				bitLength -= 4;										// Update bitLength
			} else {												// Trailing 0's
				bitLength -= 2;										// Update bitLength
			}
			msgConcat(in, &inPos, &encode, &bitLength);				// Top off the stack
		}	
		
		// Strip 0 bits from the right
		while(~decode & 1) {
			decode >>= 1;
		}	
		
		// Strip Tail
		if(decode & 1) {
			decode >>= 1;
		} else {
			return -1; // Fail Tail
		}	

		// Check Preamble
		if(decode >> 24 == 0x5d) {
			decode &= ~((uint64_t)0x5d << 24);			// Turn off Preamble
			
			// Get PIN
			int i;
			int temp;
			for(i = 0; i < 8; i++) {
				temp = ((decode >> 16) & (1 << i));
				if(temp) {
					pin |= (1 << (7 - i));
				}
			}
			decode &= ~((decode >> 16) << 16);			// Turn off PIN
			out[0] = pin;
			
			// Get Command
			// Check to see if inverse matches command
			if((decode >> 8) == ((decode & ~((decode >> 8) << 8)) ^ 0xff)) {
				decode >>= 8;
				out[1] = decode;
			} else {
				return -1; // Failed Command
			}
		} else {
			return -1; // Failed Preamble
		}
	} else {
		return -1; // Failed Sync
	}	
}

int main() {
	// Variables
	uint8_t returnValue[LEN];
	uint8_t out[2] = {0,0};

	
	// Known Values
	uint8_t msg[LEN] = {0xFF, 0xFF, 0x00, 0xa2, 0x88, 0x8a, 0x2a, 0xaa, // Skip PIN:000
		           0xa2, 0x2a, 0x28, 0xaa, 0x22, 0x8a, 0x20, 0x00, 0x00};
	uint8_t msg2[LEN] = {0xFF, 0xFF, 0x00, 0xa2, 0x88, 0x8a, 0x2a, 0xaa, // Edit Queue PIN:000
			   0xa8, 0x8a, 0xaa, 0x2a, 0x22, 0x22, 0x20, 0x00, 0x00};
	
	
	// TEST 1
	printf("===Test 1 Skip for PIN 000===\n");
	encode(returnValue, commands[4], 0);
	

	// Step each char and cmp to msg
	int i;
	for(i =0; i < LEN; i++) {
		assert(msg[i] == returnValue[i]);
	}
	printf("PASS!\n");

	// TEST 2
	printf("\n===Test 2 Edit Que for PIN 000===\n");
	encode(returnValue, commands[3], 0);

	// Step each char and cmp to msg2
	for(i =0; i < LEN; i++) {
		assert(msg2[i] == returnValue[i]);
	}
	printf("PASS!\n");

	// TEST 3
	// Encode all 32 Commands and 256 PINs
	printf("\n==Test 3 Encode All 32 Commands and 256 PINs==\n"); 
	for(i=0; i < 255; i++) {
		int j;
		for(j = 0; j < 32; j++) {
			encode(returnValue, commands[j], i);	
			decode(returnValue, out);
			assert(out[0] == i && out[1] == commands[j]);
		}
	}
	printf("PASS!\n");

	return (0);
}

#endif

