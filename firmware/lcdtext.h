/*! \file lcdtext.h
  \brief LCD text library.
*/

//! Draw a hex digit to the LCD.
void lcd_digit(int pos, int digit);
//! Draw a character to the LCD.
void lcd_char(int pos, char c);
//! Clear a digit of the LCD.
void lcd_cleardigit(int pos);
//! Draw a number to the LCD.
void lcd_number(long num);
//! Draw a string to the LCD.
void lcd_string(const char *str);
//! Draw a hex number to the LCD.
void lcd_hex(long num);


//Symbols

//! Turn the colon on or off.
void setcolon(int on);
//! Turn the period of a digit on or off.
void setperiod(int digit, int on);
//! Zeroes all the periods.
void clearperiods();
//! Set the AM symbol.
void setam(int on);
//! Set the PM symbol.
void setpm(int on);
//! Set the multiplication symbol.
void setmult(int on);
//! Set the minus symbol.
void setminus(int on);
//! Set the plus symbol.
void setplus(int on);
//! Set the division symbol.
void setdivide(int on);
