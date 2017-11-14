/*! \file lcd.h
  \brief Low-level LCD functions.
*/


/* IO ports. */

//! Active LCD memory.
extern volatile unsigned char *lcdm;
//! Buffered LCD memory.
extern volatile unsigned char *lcdbm;


/* Handy LCD functions for use in other modules.*/

//! Initialize the LCD unit.
extern void lcd_init();
//! Zero all pixels of the display.
extern void lcd_zero();
//! Call this before drawing the application.
extern void lcd_predraw();
//! Call this after drawing the application.
extern void lcd_postdraw();

//! Flicker a day-of-week segment to indicate CPU load.
extern int flickermode;
