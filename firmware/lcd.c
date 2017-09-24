/* Minimal LCD example, forked from a reference by G. Larmore at TI.
   The LCD pinout doesn't match ours yet, and will need to be
   modified to fit.  */

#include <msp430.h>

char *lcdm=&LCDM1;
char *lcdbm=&LCDBM1;

//! Clears the LCD memory and blink memory.
int lcd_zero(){
  int i=0;
  
  for(i=0; i<=13; i++){
    lcdm[i]=0;
    lcdbm[i]=0;
  }
}



//! Initialize the LCD memory and populate it with sample text.
int lcd_init() {
  int i;
  
  // Select LCD COM pins
  P5SEL |= (/*BIT5 |*/ BIT6 | BIT7);
  P5DIR |= (/*BIT5 |*/ BIT6 | BIT7);

  // Configure LCD_B
  // LCD_FREQ = ACLK/32/4, LCD Mux 4, turn on LCD
  // Charge pump generated internally at 3.44V, external bias (V2-V4) generation
  // Internal reference for charge pump
  LCDBCTL0 =  (LCDDIV0 + LCDDIV1 + LCDDIV2 + LCDDIV3 + LCDDIV4)| LCDPRE0 | LCD4MUX | LCDON | LCDSON;
  LCDBVCTL = LCDCPEN | VLCD_3_44;
  LCDBCTL0 |= LCDON + LCDSON;
  REFCTL0 &= ~REFMSTR;
  
  //Select LCD Segments 0-9
  LCDBPCTL0 = 0xFFFF;
  LCDBPCTL1 = 0xFFFF;

  //LCD Memory
  lcd_zero();
  for(i=0;i<13;i++)
    lcdbm[i]=0xFF;

  //Seems unwired?
  lcdm[2]^=0x40;
  //Weird top-right thingys.
  //lcdm[9]|=0x04;
  //lcdm[0x0a]|=0x40;
  //lcdm[0x0c]|=0x01;

  //Beyond the range.
  //lcdm[0x0c]|=0x10;

}

//! LCD callback when the CPU wakes.
void lcd_wdt(){
  static int i=0, j=0;
  
  //LCDBMEMCTL ^= LCDDISP; // Enable blink memory

    
  lcd_zero();
  //lcd_hex(0xdeadbeef);
  lcd_hex(0xcafebabe);
}
