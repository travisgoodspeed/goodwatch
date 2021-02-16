/*! \file countdown.c
  
  \brief Countdown application.
   
  Simple countdown application, use the SET button to input a time to countdown from.
  Start and stop it with + or MODE and reset it with 0.

*/

#include "api.h"
#include "countdown.h"
#include "apps/clock.h"

// States
#define STATE_READY    0
#define STATE_COUNTING 1
#define STATE_SETUP    2
#define STATE_FINISHED 3

static uint8_t state = STATE_READY;

static uint16_t count;
static uint8_t hour, hourhex;
static uint8_t min, minhex;
static uint8_t sec, sechex;

static uint8_t saved_hour = 0, saved_min = 5, saved_sec = 0;

static uint8_t flicker = 0;
static uint8_t setup_digit = 0;
static uint8_t showtime = 0;

static void countdown_reset(){
  count = 0;

  hour = saved_hour;
  hourhex = int2bcd(hour);

  min = saved_min;
  minhex = int2bcd(min);

  sec = saved_sec;
  sechex = int2bcd(sec);
}

static void countdown_save(){
  saved_hour = hour;
  saved_min = min;
  saved_sec = sec;
}

//! Entry to the countdown app.
void countdown_init(){
  countdown_reset();

  state = STATE_READY;

  countdown_draw(1);
}

//! Exit from the countdown app.
int countdown_exit(){

  if (state == STATE_SETUP) {
    setup_digit--;
    if (setup_digit == 0){
      countdown_save();
      state = STATE_READY;
      countdown_draw(1);
    }
    return 1;
  }

  if (state == STATE_FINISHED) {
  countdown_reset();
  state = STATE_READY;
    countdown_draw(1);
  return 1;
  }

  if (state == STATE_COUNTING){
    state = STATE_READY;
    return 1;
  }

  //Give up without a fight when the mode button is pressed.
  return 0;
}


static void countdown_keypress_setup(char ch){
  if ((ch & 0x30) != 0x30)
    return;

  uint8_t digit = ch & 0x0f;

  switch(setup_digit) {
    case 6:
      if (digit <= 5){
        hour = hour % 10 + digit * 10;
        setup_digit--;
      }
      break;

    case 5:
      hour = hour - hour % 10 + digit;
      setup_digit--;
      break;

    case 4:
      if (digit <= 5){
        min = min % 10 + digit * 10;
        setup_digit--;
      }
      break;

    case 3:
      min = min - min % 10 + digit;
      setup_digit--;
       break;

    case 2:
      if (digit <= 5){
        sec = sec % 10 + digit * 10;
        setup_digit--;
      }
      break;

    case 1:
      sec = sec - sec % 10 + digit;

      countdown_save();

      state = STATE_READY;
      countdown_draw(1);
      break;
  }

  hourhex = int2bcd(hour);
  minhex = int2bcd(min);
  sechex = int2bcd(sec);

}

//! A button has been pressed for the countdown.
int countdown_keypress(char ch){
  showtime = 0;

  if (state == STATE_SETUP) {
    countdown_keypress_setup(ch);
    return 1;
  }
  
  switch(ch){
  case '+':  //Pause/Resume the countdown.

    if (state == STATE_COUNTING)
      state = STATE_READY;
    else if (state == STATE_READY)
      state = STATE_COUNTING;

    break;

  case '0':  //Resets the countdown.

    if (state == STATE_READY || state == STATE_FINISHED) {
      countdown_reset();
      state = STATE_READY;
      countdown_draw(1);
    }
    break;

  case '/':  //Briefly show the clock time.
    showtime = 1;
    break;

  default:
    return 1;  //Redraw the whole screen on key-up.
    break;
  }
  
  return state != STATE_COUNTING;
}


//! Draw the countdown app and handle its input.
void countdown_draw(int forced){

  if (sidebutton_set()){
    if (state == STATE_READY){
      countdown_reset();
      setup_digit = 6;
      state = STATE_SETUP;
    } else if (state == STATE_SETUP){
      countdown_save();
      state = STATE_READY;
      forced = 1;
    }
  }

  if (state == STATE_FINISHED) {
    flicker ^= 1;

    if (flicker){
      tone(2048, 180);
    }

    return;
  }


  if (state == STATE_COUNTING) {
    app_cleartimer();

    count++;

    if ((count & 3) == 3){
      if (sec == 0 && min == 0 && hour == 0){
        state = STATE_FINISHED;
       } else {
        if (sec == 0 && min != 0){
          sec = 59;
          min--;
          minhex = int2bcd(min);
        } else if (sec == 0 && min == 0){
          hour--;
          hourhex = int2bcd(hour);
          min = 59;
          minhex = int2bcd(min);
          sec = 59;
        } else {
          sec--;
        }
        sechex = int2bcd(sec);
      }
    }
  } 

  if (forced){
    lcd_zero();
  }

  if (showtime) {
    draw_time(1);
    return;
  }

  if (!forced && state != STATE_COUNTING && state != STATE_SETUP)
    return;

  lcd_digit(7, hourhex >> 4);
  lcd_digit(6, hourhex & 0xF);
  lcd_digit(4, minhex >> 4);
  lcd_digit(3, minhex & 0xF);
  lcd_digit(1, sechex >> 4);
  lcd_digit(0, sechex & 0xF);

  setcolon((count >> 1) & 1);

  if (state == STATE_SETUP) {
    flicker ^= 1;

    if (flicker){
      switch(setup_digit){
      case 6:
        lcd_cleardigit(7); break;
      case 5:
        lcd_cleardigit(6); break;
      case 4:
        lcd_cleardigit(4); break;
      case 3:
        lcd_cleardigit(3); break;
      case 2:
        lcd_cleardigit(1); break;
      case 1:
        lcd_cleardigit(0); break;
      }
    }
  } 
}
