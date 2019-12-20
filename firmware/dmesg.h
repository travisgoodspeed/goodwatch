/*! \file dmesg.h
  \brief Kernel debug message buffer.
*/

#include <stdint.h>
#include "printf.h"

#define DMESGLEN 2048

//! Ought to be 0xdeadbeef except after power loss.
extern uint32_t dmesg_magic;
//! Index within that buffer.
extern uint16_t dmesg_index;
//! Buffer itself.
extern char *dmesg_buffer;

//! Clears the dmesg buffer.
void dmesg_clear();

//! I ain't never initialized a buffer that didn't need initializin'.
void dmesg_init();


//! Putc implementation for the printf library.
void dmesg_putc(void* p, char c);

