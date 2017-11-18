/*! \file dmesg.c
  \brief Kernel debug message buffer.
*/

#include<stdio.h>
#include<string.h>

#define DMESGLEN 1024

/* These three buffers are declared to be in the .noinit section so
   that a reboot will not wipe the buffer.  Because the memory will be
   corrupted when power is lost, we manually wipe the buffer at
   startup if the magic is corrupted or if the index is unreasonable.
 */

//! DMESG buffer itself.
char dmesg_buffer[DMESGLEN] __attribute__ ((section (".noinit")));
//! Index within that buffer.
int dmesg_index __attribute__ ((section (".noinit")));
//! Ought to be 0xdeadbeef except after power loss.
uint32_t dmesg_magic __attribute__ ((section (".noinit")));

//! Writes a character to the dmesg buffer.
int putchar(int c){
  return dmesg_buffer[dmesg_index++%DMESGLEN]=(char) c;
}

//! Clears the dmesg buffer.
void dmesg_clear(){
  memset(dmesg_buffer, 0, DMESGLEN);
  dmesg_index=0;
  dmesg_magic=0xdeadbeef;
}

//! I ain't never initialized a buffer that didn't need initializin'.
void dmesg_init(){
  if(dmesg_magic!=0xdeadbeef || dmesg_index>=DMESGLEN)
    dmesg_clear();
}
