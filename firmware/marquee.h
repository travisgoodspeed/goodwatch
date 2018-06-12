/*! \file marquee.h
  \brief Scroll text on screen atop LCD contents.
  Blame qguv for all bugs in this module.
*/

/*! Set the marquee to be drawn atop whatever's on the LCD.
    If a marquee is already going, it will be replaced. */
void marquee(const char * const s);

//! Disable a running marquee
void stop_marquee(void);

//! Blit the marquee to the screen (called by LCD module)
void draw_marquee(void);
