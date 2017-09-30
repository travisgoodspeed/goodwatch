/* This should be the only header file included by applications.  It
   should allow for general I/O functions, but nothing internal to a
   driver.  (But of course it includes too much for now.)
*/

#include "lcd.h"
#include "lcdtext.h"
#include "keypad.h"
#include "apps.h"
#include "rtc.h"
#include "sidebutton.h"

int post();
