/*! \file marquee.c
  \brief Scroll text on screen atop LCD contents.
  Blame qguv for all bugs in this module.
*/

#include <string.h>

#include "lcdtext.h"

static char marquee_buf[32];
static int marquee_len = 0;
static int marquee_startdigit;
static int marquee_skip;

#define MIN(X, Y) ((Y) < (X) ? (Y) : (X))

/*! Set the marquee to be drawn atop whatever's on the LCD.
    If a marquee is already going, it will be replaced. */
void marquee(const char * const s)
{
    marquee_len = strlen(s);
    if (marquee_len > (int) sizeof(marquee_buf))
        marquee_len = sizeof(marquee_buf);

    memcpy(marquee_buf, s, marquee_len);

    marquee_startdigit = 0;
    marquee_skip = 0;
}

//! Disable a running marquee
void stop_marquee(void)
{
    marquee_len = 0;
}

//! Blit the marquee to the screen (called by LCD module)
void draw_marquee(void)
{
    if (!marquee_len) return;

    int i;
    int digit;
    for (i = 0; i < MIN(marquee_len, marquee_startdigit + 1); i++) {
        digit = marquee_startdigit - i;
        lcd_char(digit, marquee_buf[marquee_skip + i]);
        setperiod(digit, 0);
        if (digit == 5)
            setcolon(0);
    }

    if (marquee_startdigit != 7) {
        marquee_startdigit++;
    } else {
        marquee_skip++;
        marquee_len--;
    }
}
