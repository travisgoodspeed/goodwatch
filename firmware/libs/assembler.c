/*! \file assembler.c
  \brief Tiny MSP430 disassembler library.
  
  This is a handy little MSP430 disassembler, intended for use on the
  GoodWatcher.  Because the output is a 7-segment display with just
  eight digits, the result is sometimes a little weird.
  
  For testing, you can also compile it for Unix, but you'd *REALLY* be
  better off with Radare2, IDA, or even objdump.

  --Travis

*/

#include<stdint.h>
#include<string.h>
#include<stdio.h>

//! Conditions from a jump instruction.
static enum {JNZ, JEQ, JNC, JC, JN, JGE, JL, JMP} jumpcond;
//! Strings of the jump instructions.
static const char *jumpcondstr[]={
  "jnz", "jeq", "jnc", "jc", "jn", "jge", "jl", "jmp"
};
//! Target of the most recently disassembled jump.
static uint16_t jumptarget=0x1337;
//! Address of the current instruction.
static uint16_t address;
//! Instruction type.
static enum {ERROROP, ONEOP, TWOOP, JUMPOP, EMUOP} type=ERROROP;

//! Opcode name as a string.
static const char *opstr;
//! Source and destination registers.
static int src,dst;
//! Immediate values.
static int imm0, imm1;
//! Addressing modes.
static int as, ad;
//! 1 for byte, 0 for word.
static int bw;
//! Disassembled string, to display on 7-seg display.
static char asmstr[9];

//! Disassemble an instruction into the local buffer.
void asm_dis(uint16_t adr, uint16_t ins,
	     uint16_t immediate0, uint16_t immediate1){
  /* This function disassembles an instruction, storing its opcode,
     operands, and other information in local static buffers for
     near-term use.  Because it's intended to run in a
     microcontroller, it uses a single static buffer and no heap.
  */

  //Immediates may or may not be used.
  imm0=immediate0;
  imm1=immediate1;

  //By default, we know nothing.
  type=ERROROP;
  address=adr;
  bw=0;
  as=ad=src=dst=-1;
  opstr="err";
  //Clear the string buffer.
  memset(asmstr,' ',8);


  //First, we take care of some emulated instructions.  We don't
  //support all of them.
  switch(ins){
  case 0x4303: //NOP
    type=EMUOP;
    opstr="nop";
    break;
  case 0x4130: //RET
    type=EMUOP;
    opstr="ret";
    break;
  default:
    type=ERROROP;
  }
  if(type==EMUOP)
    return;
	   
  //Single operator instructions.
  if((ins&0xFC00)==0x1000){
    type=ONEOP;
    src=dst=ins&0x000F;//Last nybble is the opcode.
    as=(ins&0x0030)>>4;
    bw=(ins&0x0040)?1:0;

    switch((ins>>7)&7){
    case 0: //RRC Rotate Right Carry
      opstr="rrc";
      break;
    case 1: //SWPB Swap Bytes
      opstr="swb";
      break;
    case 2: //RRA
      opstr="rra";
      break;
    case 3: //SXT Sign Extend
      opstr="sxt";
      break;
    case 4: //PUSH
      opstr="psh";
      break;
    case 5: //CALL
      opstr="cal";
      break;
    case 6: //RETI
      opstr="rti";
      break;
    case 7: //MSP430X extend instructions.
      /* For now, we only support the classic MSP430 instruction set.
	 We'll add support for extended instructions later, but return
	 a soft error for now.
       */
      opstr="err";
      break;
    }
    
    return;
  }

  //Conditional jump.
  if((ins&0xE000)==0x2000){
    type=JUMPOP;
    jumpcond=(ins>>10)&7;
    opstr=jumpcondstr[jumpcond];
    //Ten-bit offset, sign extended.
    jumptarget=((ins&0x3FF)<<1);
    jumptarget|=(ins&0x200)?0xF800:0;
    //Relative jump, so we add the PC and 2.
    jumptarget+=adr+2;

    return;
  }

  /* Two-operand instructions haven't got a specific prefix, so we
     test the top nybble against a list.
  */
  switch(ins&0xF000){
  case 0x4000: //MOV
    type=TWOOP;
    opstr="mov";
    break;
  case 0x5000: //ADD
    type=TWOOP;
    opstr="add";
    break;
  case 0x6000: //ADDC
    type=TWOOP;
    opstr="adc";
    break;
  case 0x7000: //SUBC
    type=TWOOP;
    opstr="sbc";
    break;
  case 0x8000: //SUB
    type=TWOOP;
    opstr="sub";
    break;
  case 0x9000: //CMP
    type=TWOOP;
    opstr="cmp";
    break;
  case 0xA000: //DADD
    type=TWOOP;
    opstr="dad";
    break;
  case 0xB000: //BIT
    type=TWOOP;
    opstr="bit";
    break;
  case 0xC000: //BIC
    type=TWOOP;
    opstr="bic";
    break;
  case 0xD000: //BIS
    type=TWOOP;
    opstr="bis";
    break;
  case 0xE000: //XOR
    type=TWOOP;
    opstr="xor";
    break;
  case 0xF000: //AND
    type=TWOOP;
    opstr="and";
    break;
  }
  if(type==TWOOP){
    src=((ins&0x0F00)>>8);
    dst=(ins&0x000F);
    bw=((ins&0x0040)?1:0);
    as=((ins&0x0030)>>8);
    ad=((ins&0x0080)?1:0);
    return;
  }
}

#ifndef STANDALONE
#include "api.h"
//! Prints the instruction to the watch LCD.
void asm_show(){
  int i;
  
  memset(asmstr,' ',8);
  strcpy(asmstr,opstr);
  
  switch(type){
  default:
    strcpy(asmstr,"error   ");
    break;
  case JUMPOP:
    for(i=0;i<4;i++){
      lcd_digit(i,(jumptarget>>(4*i))&0xF);
    }
    break;
  case TWOOP:
    /* printf("%04x: %s r%d, r%d\n", */
    /* 	   address, */
    /* 	   opstr, */
    /* 	   src, */
    /* 	   dst); */
    setam((as>>1)&1);
    setpm(as&1);
    setdivide(ad);

    lcd_digit(3,src/10);
    lcd_digit(2,src%10);
    setperiod(2,1);
    lcd_digit(1,dst/10);
    lcd_digit(0,dst%10);
    
    
    break;
  case ONEOP:
    /* printf("%04x: %s r%d\n", */
    /* 	   address, */
    /* 	   opstr, */
    /* 	   src); */
    setam((as>>1)&1);
    setpm(as&1);
    
    if(as==3 && src==0){
      /* Immediate value. */
      lcd_digit(3, (imm0>>12) & 0xF);
      lcd_digit(2, (imm0>>8) & 0xF);
      lcd_digit(1, (imm0>>4) & 0xF);
      lcd_digit(0, (imm0) & 0xF);
    }else{
      /* Source and dest are the same, so just show once. */
      lcd_digit(1,dst/10);
      lcd_digit(0,dst%10);
    }
    break;
  case EMUOP:
    //For now, emu ops are just the op string.
    break;
  }
  
  lcd_string(asmstr);
}
#endif


#ifdef STANDALONE

#include<stdio.h>
#include<assert.h>

//! Print the current instruction to the unix console, for testing.
void asm_print(){
  switch(type){
  default:
    printf("ERROR\n");
    break;
  case JUMPOP:
    printf("%04x: %s 0x%04x\n",
	   address,
	   opstr, 
	   jumptarget);
    break;
  case TWOOP:
    printf("%04x: %s r%d, r%d\n",
	   address,
	   opstr,
	   src,
	   dst);
    break;
  case ONEOP:
    if(as==3 && src==0){
      /* Immediate value, not a register. */
      printf("%04x: %s 0x%04x\n",
	     address,
	     opstr,
	     imm0);
    }else{
      printf("%04x: %s r%d\n",
	     address,
	     opstr,
	     src);
    }
    break;
  case EMUOP:
    printf("%04x: %s\n",
	   address,
	   opstr);
    break;
  }
}

//! Unix command-line tool for testing.
int main(){
  printf("Testing the MSP430 disassembler:\n");

  //3FFF is an unconditional jump to self.
  asm_dis(0xdead, 0x3FFF, 0, 0);
  asm_print();
  assert(jumptarget==0xdead);
  assert(type==JUMPOP);

  //2002 is jnz     $+6 
  asm_dis(0x013a, 0x2002, 0, 0);
  asm_print();
  assert(jumptarget==0x0140);
  assert(type==JUMPOP);

  //430F is mov r14, r15
  asm_dis(0, 0x4e0F, 0, 0);
  asm_print();
  assert(type==TWOOP);
  assert(!strcmp(opstr,"mov"));

  //4130 is a RET, emulated by MOV @SP+,PC.
  asm_dis(0x0, 0x4130, 0, 0);
  asm_print();
  assert(type==EMUOP);
  assert(!strcmp(opstr,"ret"));

  //4303 is a NOP, emulated by MOV #0,R3
  asm_dis(0x0, 0x4303, 0, 0);
  asm_print();
  assert(type==EMUOP);
  assert(!strcmp(opstr,"nop"));

  //110f is "rra r15"
  asm_dis(0, 0x110f, 0, 0);
  asm_print();
  assert(type==ONEOP);
  assert(!strcmp(opstr,"rra"));

  //12b0 is a call to the first immediate.
  asm_dis(0, 0x12b0, 0xdead, 0);
  asm_print();
  assert(type==ONEOP);
  assert(!strcmp(opstr,"cal"));
  


  //4134 is "pop r11"
  /* POPS are emulated instructions, done by memory offsets on a MOV
     from R1.
   
  printf("\nThis should be pop r11, which is emulated by a MOV.\n");
  asm_dis(0, 0x413B, 0, 0);
  asm_print();
  assert(type==EMUOP);
  assert(!strcmp(opstr,"pop"));
  */

  /*
  //930f is "tst r15"
  asm_dis(0, 0x930f);
  asm_print();
  assert(type==ONEOP);
  assert(!strcmp(opstr,"tst"));
  */

  

  return 0;
}
#endif
