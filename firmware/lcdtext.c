/*! \file lcdtext.c
  \brief LCD font and text drawing functions.
 */
#include "lcd.h"
#include "lcdtext.h"
#include "optim.h"

/* Digits look like this, and we index them with 0 being the
   leftmost.
     
     AAAAA
    F     B
    F     B
    F     B
     GGGGG
    E     C
    E     C
    E     C
     DDDDD  dp

 */


//! This maps the segments of each digit.
const int lcdmap[10][8]={
// A,      B,      C,      D,      E,      F,      G,     dp         digit
  {0x0b04, 0x0b40, 0x0b20, 0x0b01, 0x0a10, 0x0a20, 0x0b02, 0x0b10}, //0
  {0x0940, 0x0a04, 0x0a02, 0x0910, 0x0901, 0x0902, 0x0920, 0x0a01}, //1
  {0x0804, 0x0840, 0x0820, 0x0801, 0x0710, 0x0720, 0x0802, 0x0810}, //2
  {0x0640, 0x0704, 0x0702, 0x0610, 0x0601, 0x0602, 0x0620, 0x0701}, //3
  {0x0504, 0x0540, 0x0520, 0x0501, 0x0410, 0x0420, 0x0502, 0x0510}, //4
  {0x0c02, 0x0404, 0x0402, 0x0310, 0x0302, 0x0304, 0x0340, 0x0401}, //5
  {0x0204, 0x0220, 0x0210, 0x0201, 0x0110, 0x0120, 0x0202, 0x0301}, //6
  {0x0040, 0x0104, 0x0102, 0x0010, 0x0001, 0x0002, 0x0020, 0x0101}, //7
};

//These are the fragments of the day of the week: 0x0904, 0x0a40, 0x0c01
//0x0c10 is beyond the screen.

//! Bit flags for each of the eight segments.
enum lcdmappos {A=1, B=2, C=4, D=8, E=0x10, F=0x20, G=0x40, DP=0x80};


//! Font for numbers.
const int numfont[]={
  A|B|C|D|E|F,   //0
  B|C,           //1
  A|B|G|E|D,     //2
  A|B|G|C|D,     //3
  F|G|B|C,       //4
  A|F|G|C|D,     //5
  A|F|G|E|C|D,   //6
  A|B|C,         //7
  A|B|C|D|E|F|G, //8
  A|B|G|F|C|D,   //9
  A|F|B|G|E|C,   //A
  F|E|G|C|D,     //B
  A|F|E|D,       //C
  E|G|C|D|B,     //D
  A|F|E|G|D,     //E
  A|G|F|E        //F
};
//! Font for letters.
const int letterfont[]={
  /* This font begins at 0x41 hex in the ASCII table, rendering
     letters as best they can be on the 7-segment display.
  */
  A|F|B|G|E|C,   //A
  F|E|G|C|D,     //B
  A|F|E|D,       //C
  E|G|C|D|B,     //D
  A|F|E|G|D,     //E
  A|G|F|E,       //F
  A|F|G|E|C|D,   //G
  F|G|E|C,       //h
  F|E,           //I, distinguished from a 1 by being on the left side.
  E|B|C|D,       //J
  F|E|B|C|G|DP,  //K, distinguished from an X by the DP.
  F|E|D,         //L
  A|E|C,         //M  (Less nerdy than mu, but more readable.)
  E|G|C,         //n
  A|B|C|D|E|F,   //O
  F|A|B|G|E,     //P
  F|A|B|C|D|E|DP,//Q
  E|G,           //R
  A|F|G|C|D,     //S  (Looks like a 5.)
  F|E|G|D,       //T
  E|D|C,         //U  (Like a lowercase V)
  F|E|D|C|B,     //V  (Like U.  Blame Rome.)
  F|B|D,         //W  (Inverted M)
  F|G|E|B|C,     //X
  F|G|B|C|D,     //Y  
  A|B|G|E|D      //Z
};

//! Sets a pixel.
#define DRAWPOINT(todraw) lcdm[todraw>>8]|=todraw&0xFF
//! Clears a pixel.
#define CLEARPOINT(todraw) lcdm[todraw>>8]&=~(todraw&0xFF)


//! Draws one LCD digit.
void lcd_digit(int pos, int digit){
  int segments=numfont[digit];
  int bit;
  for(bit=0;bit<8;bit++){
    if(segments&(1<<bit))
      DRAWPOINT(lcdmap[pos][bit]);
    else
      CLEARPOINT(lcdmap[pos][bit]);
  }
}

//! Draws one LCD character
void lcd_char(int pos, char c){
  int segments;//=numfont[digit];
  int bit;

  //Numbers handled by another function until we unify the font.
  if(c>='0' && c<='9'){
    lcd_digit(pos, c&0x0F);
    return;
  }else if(c==' '){
    lcd_cleardigit(pos);
    return;
  }else if(c=='-'){
    lcd_cleardigit(pos);
    DRAWPOINT(lcdmap[pos][6]); //Set the G segment.
    return;
  }else if(c=='.'){
    lcd_cleardigit(pos);
    setperiod(pos,1);
    return;
  }
  
  c&=~0x20;
  segments=letterfont[c-'A'];
    
  for(bit=0;bit<8;bit++){
    if(segments&(1<<bit))
      DRAWPOINT(lcdmap[pos][bit]);
    else
      CLEARPOINT(lcdmap[pos][bit]);
  }
}

//! Clears one LCD digit.
void lcd_cleardigit(int pos){
  int bit;
  for(bit=0;bit<8;bit++)
      CLEARPOINT(lcdmap[pos][bit]);
  
}

//! Draws a string to the LCD.
void lcd_string(const char *str){
  int i=7;//LCD position.
  while(i>=0 && *str){
    lcd_char(i--, *str++);
  }
}

//! Draws a decimal number on the screen.
void lcd_unumber(long num){
  static long oldnum=0;
  static unsigned long bcd=0;

  if (num > 99999999) {
    lcd_string("overflow");
    return;
  }

  /* This conversion takes too long at 32kHz, so we cache the last
     value for rendering. */
  if(oldnum==num){
    lcd_hex(bcd);
    return;
  }

  oldnum = num;
  bcd = l2bcd(num);
  lcd_hex(bcd);
}


//! Draws a decimal number on the screen.
void lcd_number(long num){
  if(num<0){
    lcd_unumber(0-num);
    lcd_char(7,'-');
  }else{
    lcd_unumber(num);
  }
}

//! Draws hex on the screen.
void lcd_hex(long num){
  /* So in an ideal world, we'd have characters arranged nicely into
     LCDM[] bytes as some development tools do, but in the real world,
     we're pretty much stuck with them as they are physically
     arranged.
     
     This function takes a buffer of eight hex characters and displays
     them on the screen.
   */
  int i;
  
  
  for(i=0;i<8;i++)
    lcd_digit(i,(num>>(4*i))&0xf);
}

//! Activates a period, or turns it off.
void setperiod(int digit, int on){
  if(on)             //On
    DRAWPOINT(lcdmap[digit][7]);
  else               //Off
    CLEARPOINT(lcdmap[digit][7]);
}

//! Zeroes all the periods.
void clearperiods(){
  int i;
  for(i=0; i<8; i++)
    setperiod(i,0);
}


//! Activates the colon.  2 for invert.
void setcolon(int on){
  if(on==2)          //Blink
    lcdm[3]^=0x20;
  else if(on==1)     //On
    lcdm[3]|=0x20;
  else               //Off
    lcdm[3]&=~0x20;  
}

//! Activates the am.  2 for invert.
void setam(int on){
  if(on==2)          //Blink
    lcdm[0]^=0x04;
  else if(on==1)     //On
    lcdm[0]|=0x04;
  else               //Off
    lcdm[0]&=~0x04;  
}

//! Activates the pm.  2 for invert.
void setpm(int on){
  if(on==2)          //Blink
    lcdm[1]^=0x40;
  else if(on==1)     //On
    lcdm[1]|=0x40;
  else               //Off
    lcdm[1]&=~0x40;  
}

//! Activates the mult sign. 2 for invert.
void setmult(int on){
  if(on==2)          //Blink
    lcdm[4]^=0x40;
  else if(on==1)     //On
    lcdm[4]|=0x40;
  else               //Off
    lcdm[4]&=~0x40;  
}

//! Activates the minus sign. 2 for invert.
void setminus(int on){
  if(on==2)          //Blink
    lcdm[6]^=0x04;
  else if(on==1)     //On
    lcdm[6]|=0x04;
  else               //Off
    lcdm[6]&=~0x04;  
}

//! Activates the plus sign. 2 for invert.
void setplus(int on){
  if(on==2)          //Blink
    lcdm[7]^=0x40;
  else if(on==1)     //On
    lcdm[7]|=0x40;
  else               //Off
    lcdm[7]&=~0x40;  
}

//! Activates the divide sign. 2 for invert.
void setdivide(int on){
  if(on==2)          //Blink
    lcdm[0xc]^=0x04;
  else if(on==1)     //On
    lcdm[0xc]|=0x04;
  else               //Off
    lcdm[0xc]&=~0x04;  
}


