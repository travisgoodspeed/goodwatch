/* IO ports. */
extern volatile unsigned char *lcdm;
extern volatile unsigned char *lcdbm;


/* Handy LCD functions for use in other modules.*/

extern void lcd_init();
extern void lcd_zero();
extern void lcd_predraw();
extern void lcd_postdraw();

//Flicker a day-of-week segment to indicate CPU load.
extern int flickermode;
