/*! \file beats.h
  \brief Beats Application
  
*/

#include <msp430.h>
#include "api.h"
#include "libs/beats.h"
#include <stdio.h>

#ifndef CURRENT_UTC_OFFSET
#error "CURRENT_UTC_OFFSET is undefined in config.h"
#endif

static int ticks;
static int setting_offet=0;

static int16_t utc_offset = CURRENT_UTC_OFFSET;

//! Initialize the beats.
void beats_init() {
    ticks = -1;

};
//! Exit to the next application.
int beats_exit() {
    return 0;
};

static void draw_setting_offset() {
    lcd_cleardigit(7);
    lcd_char(6, 'u');
    lcd_char(5, 't');
    lcd_char(4, 'c');
    lcd_cleardigit(3);
    lcd_char(2, utc_offset > -1 ? ' ' : '-');
    uint16_t unsigned_offset = utc_offset > 0 ? utc_offset : (0 - utc_offset);
    lcd_digit(1, (unsigned_offset / 10) % 10);
    lcd_digit(0, unsigned_offset % 10);
}

static void draw_beats(int forced) {
    ticks++;
    // don't re-draw unless 30 seconds have passed
    // a "beat" is 86.4 seconds but I can't think of a good way to keep track
    // of how far along we are in a beat
    if (!forced && ticks < 4 * 30 && ticks != 0) {
        return;
    }
    ticks = 0;
    uint16_t beats = clock2beats(RTCHOUR, RTCMIN, RTCSEC, utc_offset);
   
    lcd_digit(7, (beats / 100) % 10);
    lcd_digit(6,(beats / 10) % 10);
    lcd_digit(5, (beats % 10));
    lcd_cleardigit(4); //Space
    lcd_cleardigit(3); //Space
    lcd_char(2, 'b');
    lcd_char(1, 't');
    lcd_char(0, 's');
};

//! Draw the beats.
void beats_draw(int forced) {
    if(sidebutton_set()){
        setting_offet=!setting_offet;
        forced = 1;
    }

    if (setting_offet) {
        draw_setting_offset();
    } else {
        draw_beats(forced);
    }
}

//! A button has been pressed for the beats.
int beats_keypress(char ch) {
    if(setting_offet) {
        switch(ch) {
        case '-':
            utc_offset--;
            return 1;
        case '+':
            utc_offset++;
            return 1;
        default:
            return 0;
        }
    } else {
        return 0;
    }
};

//! A button has been pressed in fallthrough mode
int beats_fallthrough_keypress(char ch) {
    if(ch == '1') {
        beats_draw(1);
    }

    return 1;
}
