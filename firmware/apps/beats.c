/*! \file beats.h
  \brief Beats Application
  
*/

#include <msp430.h>
#include "api.h"
#include "libs/beats.h"
#include <stdio.h>

int ticks;

//! Initialize the beats.
void beats_init() {
    ticks = -1;

};
//! Exit to the next application.
int beats_exit() {
    return 0;
};

//! Draw the beats.
void beats_draw() {
    ticks++;
    // don't re-draw unless 30 seconds have passed
    // a "beat" is 86.4 seconds but I can't think of a good way to keep track
    // of how far along we are in a beat
    if (ticks < 4 * 30 && ticks != 0) {
        return;
    }
    ticks = 0;
    uint16_t beats = clock2beats(RTCHOUR, RTCMIN, RTCSEC);
   
    lcd_digit(7, (beats / 100) % 10);
    lcd_digit(6,(beats / 10) % 10);
    lcd_digit(5, (beats % 10));
    lcd_cleardigit(4); //Space
    lcd_cleardigit(3); //Space
    lcd_char(2, 'b');
    lcd_char(1, 't');
    lcd_char(0, 's');
};

//! A button has been pressed for the beats.
int beats_keypress(char ch) {
    return 1;
};