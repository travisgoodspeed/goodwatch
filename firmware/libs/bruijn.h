/*! \file bruijn.h
  \brief Bruijn sequence header.

*/


#include <stdint.h>
#include <math.h>


// size should be 2**BITS+(BITS-1), but we're limited by chip
// to 255 and further limited by stack size
//#define MAXLEN 255 // maximum packet length
#define MAXLEN 128 // maximum packet length
#define MAXBITS 13

#define G_HZ 0   // Hz
#define G_BAUD 1 // baudrate
#define G_BITS 2 // bits
#define G_LEN 3  // each bit code is represented by multiple bits, so what is that length
#define G_TRI 4  // is this trinary? (3-bit garages)
#define G_0 5    // eg, if bit "0" is "1000", then 0x8 (1000)
#define G_1 6    // bit 1's representation
#define G_2 7    // only if trinary



#define MHZ 26

#ifdef LOCAL
uint16_t PA_TABLE1, PA_TABLE0, FREQ0, FREQ1, FREQ2, CHANNR, MDMCFG3, MDMCFG4;
uint8_t realbuf[MAXLEN+1];
#else
extern uint8_t realbuf[MAXLEN+1];
#endif

void rftx();
void waitForTx();
void db_send();
void setBit(uint8_t *A, uint16_t b, uint8_t val);
uint8_t testBit(uint8_t *A, uint16_t b);
void db(uint8_t t, uint8_t p);
void convert_bits();
void de_bruijn();
void setFreq();
void setBaud();
void doTx();




const static struct {
  const char name[9];
  uint32_t hz;
  uint32_t baud;
  uint8_t bits;
  uint8_t len;
  uint8_t tri;
  uint32_t b0;
  uint32_t b1;
  uint32_t b2;
} garages[] = {
//        HZ, BAUD, BITS, LEN, TRI, B0,  B1,  B2

	// Stanley, Linear Multicode & Moore-o-Matic, 8 & 10 dip switch, 310MHz
	{" 310 mhz", 310000000, 2000,   10,   4,   0, 0x8, 0xe, 0x0},

	// Chamberlain, 7, 8 & 9 dip switch, 390MHz
	{" 390 mhz", 390000000, 2000,    9,   4,   0, 0x8, 0xe, 0x0},

	// Chamberlain Canada, 9 dip switch, 315MHz
	{" 315 mhz", 315000000, 2000,    9,   4,   0, 0x8, 0xe, 0x0},

	// NSCD is 2k baud rate but tri-state
  // 0 = 100000000100000000, 1 = 111111110100000000, 2 = 111111110111111110
	{" 318 mhz", 318000000, 2000,    9,  18,   1, 0x020100, 0x03fd00, 0x03fdfe},

};

extern uint8_t _garage_id;
#define g garages[_garage_id]

