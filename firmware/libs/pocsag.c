/*! \file pocsag.c
  \brief Efficient microcontroller implementation of POCSAG reception.
  
  This is a bare bones parser for POCSAG intended to run inside of the
  CC430F6137 and CC430F6147 chips from Texas Instruments, which
  contain a CC1101 radio core.  Sadly it does not support error
  correction or even the parity check.
  
  If you define STANDALONE, this will compile as a text case in Unix.
  I miss my big endian machines, but not so much as to keep this
  compatible with them.
  
  --Travis
*/

#include<stdint.h>
#include<string.h>

#include "pocsag.h"




uint32_t pocsag_lastid;
char pocsag_buffer[MAXPAGELEN];

//! Count of words within the batch, roughly twice the frame count.
static int wordcount=0;
//! Count of bits in the current byte.
static int bitcount=0;
//! Next incoming character.
static char newchar=0;
//! Character index.
static int bytecount=0;

void pocsag_newbatch(){
  wordcount=0;
  memset(pocsag_buffer,0x00,MAXPAGELEN);
}

//! Local function to handle alphanumeric data payloads.
static void pocsag_handledataword(uint32_t word){
  //Twenty bits of this word.
  uint32_t bits=((word&0x7FFFFFFF)>>11);
  //Index within those bits.
  int i;
  //Latest bit that we're sampling.
  int newbit;

  for(i=19; i>=0; i--){
    //This grabs the bits in line order. As in RS232, the least
    //significant bit comes first.
    newbit=((bits>>i) & 1) ? 1 : 0;
    bitcount++;
    
    //Shift that into the top of the new char.
    newchar = newchar>>1;
    newchar|=(newbit<<6);
    
    //When the character is complete, it's already been rearranged to
    //the right order.
    if(bitcount==7){
      //Record the character.
      pocsag_buffer[bytecount++]=newchar;
      pocsag_buffer[bytecount]=0;
      //Safety first.
      if(bytecount>MAXPAGELEN-1)
        bytecount=0;
      //Clear our counts to start again.
      bitcount=0;
      newchar=0;
    }
  }
}

void pocsag_handleword(uint32_t word){

  
  if(word==0x7a89c197){         //IDLE
    /* Just ignore idle frames, but don't yet return because we need
       to count them.
     */
  }else if(word&0x80000000){    //DATA
    pocsag_handledataword(word);
  }else if(!(word&0x80000000)){ //ADDRESS
    /* 18 bits of the address come from the address word's payload,
       but the lowest three bits come from the frame count, which is
       half of the word count.
     */
    pocsag_lastid=((word>>10)&0x1ffff8) | ((wordcount>>1)&7);

    //Wipe the alphanumeric message state.
    bitcount=0;
    newchar=0;
    bytecount=0;
  }

  /* Increment the word count, because we need it to decode the
     address.
   */
  wordcount++;
}


#ifdef STANDALONE

#include<stdio.h>
#include<assert.h>


//! Performs error correction on the word, as best we can.
static uint32_t pocsag_correct(uint32_t word){
  /* Ideally we'll do the error correction, but for now we just check
     parity.
   */
  if(__builtin_parityl(word)==1){
    //printf("0x%lx has bad parity.\n", word);

    //Return idle, rather than damaged frame.
    return 0x7a89c197;
  }
  
  //Looks good, so return it.
  return word;
}

//! Unix command-line tool for testing.
int main(){
  int i;

  //First, let's test that pocsag_correct() doesn't damage good frames.
  assert(pocsag_correct(0x7a89c197)==0x7a89c197); //IDLE is IDLE
  assert(pocsag_correct(0x08fa5e2b)==0x08fa5e2b); //Address isn't damaged.
  assert(pocsag_correct(0x08fa5e2c)==0x7a89c197); //Damaged address is corrected.
  
  //Initialize a new batch.
  pocsag_newbatch();
  //Run eight IDLE words.
  for(i=0; i<8; i++){
    //Wordcount must be right or the address will be wrong.
    assert(wordcount==i);
    pocsag_handleword(0x7a89c197);
  }
  
  //Now we provide the address word, and check the ID and function.
  pocsag_handleword(0x08fa5e2b);
  //Is the ID correctly identified?
  assert(pocsag_lastid==147092);

  //This will populate the string with the incoming message.
  pocsag_handleword(0xe9d25fc7);
  pocsag_handleword(0x9ae159b4);
  pocsag_handleword(0xab812aeb);
  pocsag_handleword(0x9f600572);

  //Idle as we're done.
  pocsag_handleword(0x7a89c197);

  //Did we get the right message?
  assert(!strcmp(pocsag_buffer,"KK4VCZ: Jo"));

  printf("%d: %s\n",
         pocsag_lastid,
         pocsag_buffer);
  
  //Damaged frame.
  pocsag_handleword(0x7a89f000);
  
  return 0; //Doesn't work yet.
}

#endif
