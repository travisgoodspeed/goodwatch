/*! \file dmesg.c
  \brief Kernel debug message buffer.
*/

#include<stdio.h>
#include<string.h>

#include "dmesg.h"

/* These three buffers are declared to be in the .noinit section so
   that a reboot will not wipe the buffer.  Because the memory will be
   corrupted when power is lost, we manually wipe the buffer at
   startup if the magic is corrupted or if the index is unreasonable.
 */

//! Ought to be 0xdeadbeef except after power loss.
uint32_t dmesg_magic __attribute__ ((section (".noinit")));
//! Index within that buffer.
uint16_t dmesg_index __attribute__ ((section (".noinit")));

//! DMESG buffer itself.
char *dmesg_buffer=(char*)0x2400;

//! Writes a character to the dmesg buffer.
int putchar(int c){
  dmesg_index++;
  while(dmesg_index>DMESGLEN)
    dmesg_index=dmesg_index-DMESGLEN;
  return dmesg_buffer[dmesg_index]=(char) c;
}

//! Writes a character to the dmesg buffer.
void dmesg_putc(void* p, char c){
  putchar(c);
}


//! Clears the dmesg buffer.
void dmesg_clear(){
  memset(dmesg_buffer, 0, DMESGLEN);
  dmesg_index=0;
  dmesg_magic=0xdeadbeef;
}

//! I ain't never initialized a buffer that didn't need initializin'.
void dmesg_init(){
  if(dmesg_magic!=0xdeadbeef){
    dmesg_clear();
    printf("Zeroed buffer for bad magic.");
  }
  printf("\n\n----\n");
}
