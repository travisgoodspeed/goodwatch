#include <stdint.h>
#include <stdbool.h>
#include <msp430.h>

#include "flash.h"
#include "debug.h"

#define DUMMY_ADDRESS 0xFFE0

void flash_unlock() {
    FCTL3 = FWPW;                      // clear lock
    FCTL1 = FWPW + WRT;                // enable write
}

void flash_lock(void) {
    FCTL1 = FWPW;                      // clear write
    FCTL3 = FWPW + LOCK;               // set lock
}

void flash_erase(void) {
	volatile char *dummy_ptr;          //Flash pointer

	while (FCTL3 & BUSY);

	FCTL3 = FWPW;                      // clear lock

	while (FCTL3 & BUSY);

	dummy_ptr = (char *)DUMMY_ADDRESS; // ivt start
	FCTL1 = FWPW + MERAS + ERASE;      // mass erase
	*dummy_ptr = 0;                    // dummy write to make erase complete

	while (FCTL3 & BUSY);

	FCTL3 = FWPW +  LOCK;               // set lock
}


bool flash_write_byte(uint8_t *addr, uint8_t data) {

    while (FCTL3 & BUSY);
    *addr = data;
    while (FCTL3 & BUSY);

    if (*addr != data) {
        return false;
    }
    return true;
}

bool flash_write_word(uint16_t *addr, uint16_t data) {

    while (FCTL3 & BUSY);
    *addr = data;
    while (FCTL3 & BUSY);
    if (*addr != data) {
        return false;
    }
    return true;
}

// Comparisons made in 32bit to prevent overflows
bool flash_write(uint16_t dst, uint8_t *src, uint8_t size) {
    bool r = true;

    if (dst < 0x8000 || ((uint32_t) dst + (uint32_t) size) > (uint32_t) 0x10000){
	    L("oob");
	    return 0;
    }

    for (uint32_t i = (uint32_t) dst; i < ((uint32_t) dst) + ((uint32_t) size); i++) {

        if ((dst & 0x01) || i == (uint32_t) dst + (uint32_t) size - 1) {
            r = flash_write_byte((uint8_t *)((uint16_t)i), *src);
            src += 1;
        } else {
            r = flash_write_word((uint16_t *)((uint16_t)i), *((uint16_t *)src)); // mis-alignment if rxbuffer is not aligned to 2
            src += 2;
            i++;
        }

        if (!r) {
            return r;
        }

    } 

    return r;
}
