/*! \file clock.c
  \brief Main clock application, that runs by default.
  
  This is the main clock application, which is seen by default
  whenever the watch has nothing else to do.  In addition to telling
  the time, holding a button allows you to see additional
  information, such as the date and git revision.
  
  By default, the time is drawn.  Hold 7 to run a self-test, 8 to
  display the callsign, 9 for the day of the week, / for the date, 4
  for the git tag, 5 for the date of flashing, 6 to toggle the CPU
  load indicator, 0 for the name of the current working channel.
  Hold 1 for the voltage, * for the CPU model number.
*/

#include <msp430.h>
#include <stdio.h>

#include "api.h"

#include "applist.h"





#define RTCAE (0x80) /* Real Time Clock Alarm enable */

//! If non-zero, we need to redraw the whole time.
static int redraw=0;

//! Last measured voltage.
static unsigned int vcc;

//! This hold the last character pressed.
static char lastchar=0;

//! Draws the time.
void draw_time(int always){
  /* So at first glance, this might seem a bit complicated.  Why draw
     the seconds first, and why take so many opportunities to abort
     and return without drawing anything?

     The reason is that the vast majority of CPU time is spent drawing
     the time to the LCD, and that anything we can do to shorten that
     time in each quarter-second frame will increase the watch's
     battery life.  Therefore, we only draw the seconds if they have
     changed, which lets us do nothing on three quarters of the
     frames.  Then we only draw the minutes and hours if the minutes
     have changed, saving two thirds of the digit drawing on each
     frame, and we only draw the tens digit if the ones digit is zero,
     saving us half the LCD memory writes and trable lookups during
     those frames.
     
     This could be further improved by testing hours, but as that only
     applies to one frame in 3600, it won't impact battery life very
     much.  When a button has been pressed or the mode changed, it is
     set non-zero to force the whole frame to be drawn.
     
     int2bcd() works by a ROM table lookup in bcd.o.  The '6147 also
     has a hardware BCD calculator, but as this isn't present in the
     '6137 and only costs us 64 bytes of ROM, it isn't a very high
     priority.
     
     All the digits are drawn out of order for these fancy reasons.
     Sorry about that.
   */
  
  unsigned int hour=RTCHOUR;
  static unsigned int min;
  static unsigned int sec;
  unsigned int lsec; //Lower digit of the seconds.

  //Only draw once a second, unless a button is pressed.
  if(sec==RTCSEC && !always)
    return;
  sec=RTCSEC;
  lcd_digit(0,lsec=int2bcd(sec)&0xf); //Lower digit.
  if(lsec && !always)  //Only need to draw tens digit if ones is zero.
    return;
  lcd_digit(1,int2bcd(sec)>>4);
  

  //If the minute hasn't changed, don't bother drawing it or the hour.
  if(min==RTCMIN && !always)
    return;
  min=RTCMIN;

  lcd_cleardigit(2); //Space between seconds and minutes.
  
  lcd_digit(7,int2bcd(hour)>>4);
  lcd_digit(6,int2bcd(hour)&0xf);
  lcd_cleardigit(5); //Space between hours and minutes.
  setcolon(1);
  lcd_digit(4,int2bcd(min)>>4);
  lcd_digit(3,int2bcd(min)&0xf);

  setam(hour<12);
  setpm(hour>=12);

  // get alarm status
  if (RTCAHOUR & RTCAE && RTCAMIN & RTCAE)
    setplus(1);
  else
    setplus(0);
}

//! Draws the date as yyyy.mm.dd
void draw_date(){
  unsigned int year=RTCYEAR;
  unsigned int month=RTCMON;
  unsigned int day=RTCDAY;

  lcd_digit(7,(year/1000)%10);
  lcd_digit(6,(year/100)%10);
  lcd_digit(5,(year/10)%10);
  lcd_digit(4,year%10);
  setperiod(4,1);
  setcolon(0);
  lcd_digit(3,month/10);
  lcd_digit(2,month%10);
  setperiod(2,1);
  lcd_digit(1,day/10);
  lcd_digit(0,day%10);

  setam(0);
  setpm(0);
}

static const char *daysofweek[7]={
  "  sunday",
  "  monday",
  " tuesday",
  "wednesdy",
  "thursday",
  "  friday",
  "saturday"
};

//! Draws the day of the week.
static void draw_dow(){
  lcd_string(daysofweek[RTCDOW]);
}

//! Draws the date as yyyy.mm.dd
static void draw_date_rom(){
  unsigned int year=(romsavetime[4]+(romsavetime[5]<<8)) % 4096;;
  unsigned int month=romsavetime[6];
  unsigned int day=romsavetime[7];

  lcd_digit(7,(year/1000)%10);
  lcd_digit(6,(year/100)%10);
  lcd_digit(5,(year/10)%10);
  lcd_digit(4,year%10);
  setperiod(4,1);
  setcolon(0);
  lcd_digit(3,month/10);
  lcd_digit(2,month%10);
  setperiod(2,1);
  lcd_digit(1,day/10);
  lcd_digit(0,day%10);

  setam(0);
  setpm(0);
}

//! Exit clock when the sidebutton is pressed.
int clock_exit(){
  return 0;
}

//! Plays the time as audio.
void clock_playtime(int hold){
  char buf[12];
  sprintf(buf,"%02d %02d",
          RTCHOUR, RTCMIN);
  lcd_string(buf);
  //Little delay, so we can quit early with dignity on accidental keypresses.
  audio_morse("  ", 1);
  audio_morse(buf, //Buffer to play.
              hold //Play it only so long as the button is held down.
              );
}


//! Draws the clock face in the main application.
void clock_draw(int forced){
  //Use the SET button to reconfigure the time.
  if(sidebutton_set()){
    //Wait for the button to be released.
    while(sidebutton_set());
    //Switch to the setting applet.
    app_set(&setting_applet);
  }
  
  if(!lastchar){
    // Draw the time by default, but only if no buttons pushed.
    draw_time(redraw);
    if(redraw)
      redraw--;
  }
}


//! Entry to the clock app.
void clock_init(){
  lastchar=0;
  redraw=0;
  lcd_zero();
  draw_time(1);
}



//! A button has been pressed for the clock.
int clock_keypress(char ch){
  lastchar=ch;

  /* This function is called *once* per keypress event, while the
     handlers in clock_draw() are called once per frame.  It is very
     important that anything that takes more than a frame be handled
     here, rather than in clock_draw(), which is rerun for every frame.
   */

  lcd_zero();
  redraw++;
    
  switch(ch){
  case '7':
    //Hold 7 to run the self-test after startup.  Response codes try
    //to roughly describe the fault.
    post();
    break;
  case '8':
    //8 shows the callsign.
    lcd_string(CALLSIGN);
    break;
  case '9':
    //Hold 9 to draw the day of the week.
    draw_dow();
    break;
  case '/':
    //Hold / to draw the date.
    draw_date();
    break;

  case '4':
    //4 shows the git revision.
    lcd_hex(GITHASH);
    lcd_cleardigit(7);
    break;
  case '5':
    //5 shows the git date from Flash.
    draw_date_rom();
    break;
  case '6':
    //6 toggles the CPU load indicator.
    flickermode=(flickermode?0:-1);
    lcd_string(flickermode?"FLICK ON":"FLICKOFF");
    break;
  case '*':
    //* shows the chip model number.
    lcd_string(DEVICEID==DEVICEID6137?"430F6137":"430F6147");
    break;

      
  case '=':
    /* = shows the the voltage.  To save on power, the reference is
     *   not active when idling.  This used to be the 1 button.
     */
    ref_on();
    vcc=adc_getvcc();
    ref_off();
    lcd_number(vcc);
    lcd_string("volt ");
    setperiod(2,1);
    break;

      
  case '0':
    //0 shows the name of the working channel.
    if(has_radio)
      lcd_string(codeplug_name());
    else
      lcd_string("NO RADIO");
    break;
  case '.':
    //. shows the frequency of the working channel.
    if(has_radio){
      lcd_number(codeplug_getfreq()/10);
      setperiod(5,1);
      setperiod(2,1);
    }else{
      lcd_string("NO RADIO");
    }
    break;


  case '+':
    /* + beeps the time as morse code, which might be handy
       for learning the code or when it's too dark to see the display.
     */
    clock_playtime(1);
    break;

  default:
    /* All unused buttons fall through to the handler of the selected
       submenu applet, but only the third row (1,2,3,-) is expected to
       remain unused in the long run.
     */
    return submenu_fallthrough(ch);
    break;
  }
  return 1;
}
