/*! \file assembler.h
  \brief MSP430 disassembler.
  
  These handy functions will disassemble MSP430 instructions on an
  MSP430, displaying them to the 7-segment display of a GoodWatch. 
 */

//! Process an instruction.
void asm_dis(uint16_t adr, uint16_t ins,
	     uint16_t immediate0, uint16_t immediate1);

//! Display the most recently processed instruction to the LCD.
void asm_show();
