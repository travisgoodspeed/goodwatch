/* Minimal LCD example, forked from a reference by G. Larmore at TI.
   The LCD pinout doesn't match ours yet, and will need to be
   modified to fit.  */

#include <msp430.h>

// LCD Segments
#define LCD_A    BIT4
#define LCD_B    BIT5
#define LCD_C    BIT6
#define LCD_D    BIT7
#define LCD_F    BIT0
#define LCD_G    BIT1
#define LCD_E    BIT2
#define LCD_H    BIT3

// LCD Segment Mapping for the SBLCDA4; the Casio display will be different.
const unsigned char  LCD_Char_Map[] = {
  LCD_A+LCD_B+LCD_C+LCD_D+LCD_E+LCD_F,        // '0' or 'O'
  LCD_B+LCD_C,                                // '1' or 'I'
  LCD_A+LCD_B+LCD_D+LCD_E+LCD_G,              // '2' or 'Z'
  LCD_A+LCD_B+LCD_C+LCD_D+LCD_G,              // '3'
  LCD_B+LCD_C+LCD_F+LCD_G,                    // '4' or 'y'
  LCD_A+LCD_C+LCD_D+LCD_F+LCD_G,              // '5' or 'S'
  LCD_A+LCD_C+LCD_D+LCD_E+LCD_F+LCD_G,        // '6' or 'b'
  LCD_A+LCD_B+LCD_C,                          // '7'
  LCD_A+LCD_B+LCD_C+LCD_D+LCD_E+LCD_F+LCD_G,  // '8' or 'B'
  LCD_A+LCD_B+LCD_C+LCD_D+LCD_F+LCD_G,        // '9' or 'g'
  LCD_A+LCD_B+LCD_C+LCD_E+LCD_F+LCD_G,        // 'A'    10
  LCD_A+LCD_D+LCD_E+LCD_F,                    // 'C'    11
  LCD_B+LCD_C+LCD_D+LCD_E+LCD_G,              // 'd'    12
  LCD_A+LCD_D+LCD_E+LCD_F+LCD_G,              // 'E'    13
  LCD_A+LCD_E+LCD_F+LCD_G,                    // 'F'    14
  LCD_B+LCD_C+LCD_E+LCD_F+LCD_G,              // 'H'    15
  LCD_B+LCD_C+LCD_D+LCD_E,                    // 'J'    16
  LCD_D+LCD_E+LCD_F,                          // 'L'    17
  LCD_A+LCD_B+LCD_E+LCD_F+LCD_G,              // 'P'    18
  LCD_B+LCD_C+LCD_D+LCD_E+LCD_F               // 'U'    19
};

//! Initialize the LCD memory and populate it with sample text.
int lcd_init() {
  // Select LCD COM pins
  P5SEL |= (BIT5 | BIT6 | BIT7);
  P5DIR |= (BIT5 | BIT6 | BIT7);

  // Configure LCD_B
  // LCD_FREQ = ACLK/32/4, LCD Mux 4, turn on LCD
  // Charge pump generated internally at 3.44V, external bias (V2-V4) generation
  // Internal reference for charge pump
  LCDBCTL0 =  (LCDDIV0 + LCDDIV1 + LCDDIV2 + LCDDIV3 + LCDDIV4)| LCDPRE0 | LCD4MUX | LCDON | LCDSON;
  LCDBVCTL = LCDCPEN | VLCD_3_44;
  LCDBCTL0 |= LCDON + LCDSON;
  REFCTL0 &= ~REFMSTR;
  
  //Select LCD Segments 0-9
  LCDBPCTL0 = 0x03FF;
  LCDBPCTL1 = 0x0000;

  //LCD Memory
  LCDM5 = LCD_Char_Map[15];     //H
  LCDM4 = LCD_Char_Map[13];     //E
  LCDM3 = LCD_Char_Map[17];     //L
  LCDM2 = LCD_Char_Map[17];     //L
  LCDM1 = LCD_Char_Map[0];      //0
  
  // Blink Memory 
  LCDBM5 = LCD_Char_Map[11];    //C
  LCDBM4 = LCD_Char_Map[11];    //C
  LCDBM3 = LCD_Char_Map[4];     //4
  LCDBM2 = LCD_Char_Map[3];     //3
  LCDBM1 = LCD_Char_Map[0];     //0
}

//! LCD callback when the CPU wakes.
void lcd_wdt(){
  LCDBMEMCTL ^= LCDDISP;
}
