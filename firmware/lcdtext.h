

void lcd_digit(int pos, int digit);
void lcd_char(int pos, char c);
void lcd_cleardigit(int pos);
void lcd_number(long num);
void lcd_string(char *str);
void lcd_hex(long num);


//Symbols
void setcolon(int on);
void setam(int on);
void setpm(int on);
void setmult(int on);
void setminus(int on);
void setplus(int on);
void setdivide(int on);
