/* This is the main clock application, which is seen by default
   whenever the watch has nothing else to do.  In addition to telling
   the time, holding a button allows you to see additional information,
   such as the date or the voltage.
*/

#include "api.h"

//! Entry to the clock app.
void clock_init(){
  lcd_zero();
}

//! Draws the clock face in the main application.
void clock_draw(){
  static char oldch=0;
  char ch=getchar();

  if(oldch!=ch)
    lcd_zero();
  
  switch(ch){
  case '/':
    draw_date();
    break;
  case '7':
    lcd_hex(0xdeadbeef);
    break;
  case '4':
    lcd_hex(0xcafebabe);
    break;
  case '+':
    app_next();
    break;
  case 0:
    draw_time();
    break;
  default:
    lcd_hex(ch);
  }

  oldch=ch;
}
