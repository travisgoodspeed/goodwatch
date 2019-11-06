/*! \file bruijn.c
  \brief De Bruijn sequence generator.
  
  This short library is pretty much a direct port of an example by
  Sammy Kamkar, which he published five years back for the IMME as
  part of his OpenSesame project.  Like Sammy's example, much of this
  is optimized for CC1101 radios driven by microcontrollers.
  
  Defining STANDALONE ought to let it build for desktops, and leaving
  that undefined should make it suitable for use in firmware.
  
  --Travis
*/

#include<stdint.h>
#include<string.h>
#include<math.h>

#ifdef STANDALONE
# include<stdio.h>
# include<assert.h>
#endif

#include "bruijn.h"

// we're making these global because we have limited stack and we're
// calling some functions recursively
static uint8_t a[MAXBITS+1]; // largest bit size possible
static uint16_t s, k;
static uint8_t tmpi;
static uint8_t firstTx;

// we could read from garages[] directly but this is faster
static uint8_t codelen;
static uint8_t bits, len;
static uint8_t tri;
static uint32_t b0, b1;
//#define bits g.bits
//#define len  g.len
//#define tri  g.tri
//#define b0   g.b0
//#define b1   g.b1


#ifdef LOCAL
void rftx() { }
void waitForTx() { }
void printl(uint8_t line, char *str) { }
int main() { db_send(); }
uint8_t sequence[MAXLEN];
#else
uint8_t sequence[MAXLEN];
#endif

uint8_t realbuf[MAXLEN+1];


// set or clear bit b in array A
void setBit(uint8_t *A, uint16_t b, uint8_t val){
  val ?
    (A[b / 8] |=   1 << (b % 8)) :
    (A[b / 8] &= ~(1 << (b % 8)));
}

// test bit b in array A
uint8_t testBit(uint8_t *A, uint16_t b){
  return ( ( A[b / 8] & (1 << (b % 8) ) ) != 0 );
}

// binary de bruijn sequence -samyk
void db(uint8_t t, uint8_t p){
  uint8_t j;
  
  if (t > bits){
    if (bits % p == 0){
      for (j = 1; j <= p; j++){
	setBit(sequence, s++, a[j]);
	
	// once we have 254 bytes (may reduce because running out of
	// space!), we need to tx due to size constraints
	if ((s + 1) * len >= MAXLEN * 8)
	  // transmit
	  doTx();
      }
    }
  }else{
    a[t] = a[t-p];
    
    db(t+1, p);
    for (j = a[t-p] + 1; j <= (tri ? 2 : 1); j++){
      a[t] = j;
      db(t+1, t);
    }
  }
}

// converts our bits to the actual bitstream/modulation the garage expects
void convert_bits(){
  uint8_t tlen;
  uint16_t z;
  uint32_t tb;

  //FIXME: This should not be using floating point.
  
  // pull in last X bits of last transmission
  if (!firstTx){
    // let's keep last bits (10) * len (4) since we may have broken the true code up during this delay
    for (tmpi = 0; tmpi < codelen; tmpi++){
      //printf("s=%d cl=%d len=%d realbuf[%d] = rb[%d]\n", s, codelen, len, tmpi,
      //			(int)(ceilf((s*len)/8.0)) - codelen + tmpi);
      realbuf[tmpi] = realbuf[(int)(ceilf((s*len)/8.0)) - codelen + tmpi]; //FIXME What the fuck was this?
      //realbuf[tmpi] = realbuf[((s*len)/8) - codelen + tmpi];
      //printf("realbuf[%d]=%x\n", tmpi, realbuf[tmpi]);
    }
  }else
    firstTx = 0;

  // go through each bit and convert to the garage bitstream equivalent
  // eg 0 = 1000, 1 = 1110
  for (z = k / len; z < s; z++) {
    tb = testBit(sequence, z) ? b1 : b0;

    #ifdef STANDALONE
    //Uncomment this to dump the sequence bits.
    printf("%d", tb==b1);
    #endif
    
    tlen = len;
    while (tlen--){
      // there's gotta be a better way to do this but my brain hurts right now
      setBit(realbuf, (7-(k%8))+((k/8)*8), (tb >> tlen) & 1);
      k++;
    }
  }
  
  // start 10 bits in next time (assuming 10 bit code)
  k = bits * len;
  s = bits;
}


// start the de bruijn sequence using the recursive db() function
void de_bruijn(){
  // clear a[], s
  for (tmpi = 0; tmpi <= MAXBITS; tmpi++)
    a[tmpi] = 0;
  s = 0;
  firstTx = 1;
  
  // begin recursive de bruijn
  db(1, 1);
  
  // a real de bruijn wraps, but since we can't wrap time we
  // have to send the last of the bits to complete the sequence
  for (tmpi = 0; tmpi < bits-1; tmpi++)
    setBit(sequence, s++, 0);
  
  // transmit anything left over
  doTx();
}


#ifdef STANDALONE

#include<stdio.h>
#include<assert.h>


void doTx(){

  //Wait for TX, which we can skip in simulation.

  //Modify our buffer to the bits needed on the air.
  convert_bits();

  //Begin transmitting, which we can skip in simulation.

}

uint8_t _garage_id=0;

//! Unix command-line tool for testing.
int main(){
  int i;
  
  for(i=0; i<4; i++){
    //And finally the fields of some garage door.
    bits    = g.bits;
    len     = g.len;
    tri     = g.tri;
    b0      = g.b0;
    b1      = g.b1;
    codelen = ceilf(bits * (len / 8.0));
    
    printf("%s: bits=%d, len=%d, tri=%d, b0=%x, b1=%x, codelen=%d\n",
	   g.name,
	   bits, len, tri, b0, b1, codelen);
    
    de_bruijn();
    
    printf("\n");
  }
  return 0; //All Good.
}

#endif
