/*! \file optim.c
  \brief Hardware optimisation routines in MSP430 assembly.
 */



/* Taken from Alex Mykyta's msp430 compatibility header
   https://github.com/gbkhorn/msp430lib/blob/63d9ba672e75749fa19b0958af5aebb2d977b31c/include/intrinsics_xc.h#L66

*/
#define bcd_add_long(op1, op2) \
({ \
            unsigned long int __op1 = op1;      \
            unsigned long int __op2 = op2;      \
            unsigned long int __result;         \
            __asm__("mov %A1, %A0\n\t"          \
                    "mov %B1, %B0\n\t"          \
                    "clrc\n\t"                  \
                    "dadd %A2, %A0\n\t"         \
                    "dadd %B2, %B0\n\t"         \
                    :"=r"(__result)             \
                    :"r"(__op1),"r"(__op2)      \
                    :                           \
            );                                  \
            __result; \
})

long l2bcd(long num) {
  long bcd = 0;
  int i;

  /* Screen is eight or fewer digits, so only
     the lower 27 bits matter. */
  num <<= 5;
  for (i = 0; i < 27; i++) {
    bcd = bcd_add_long(bcd, bcd);
    if (num & 0x80000000)
      bcd = bcd_add_long(bcd, 1ul);
    num <<= 1;
  }

  return bcd;
}
