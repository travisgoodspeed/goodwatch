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

#include "api.h"

#define RTCAE (0x80) /* Real Time Clock Alarm enable */

//! If non-zero, we are setting the time.
static int settingclock=0;
//! If non-zero, we need to redraw the whole time.
static int redraw=0;

//! Last measured voltage.
static unsigned int vcc;

//! This hold the last character pressed.
static char lastchar=0;

//! Draws the time.
static void draw_time(int always){
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
static void draw_date(){
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
  setcolon(0);
  lcd_digit(3,month/10);
  lcd_digit(2,month%10);
  lcd_digit(1,day/10);
  lcd_digit(0,day%10);

  setam(0);
  setpm(0);
}




//! Draws whatever is being set
static void draw_settingtime(){
  static int flicker=0;
  
  flicker^=1;

  //Zero the second hand if we're not yet to the date.  ("Hacking.")
  if(settingclock<7)
    SetRTCSEC(0);

  //First we draw the entire thing, then we blink the second being
  //set.
  if(settingclock<7)
    draw_time(1);
  else
    draw_date();

  switch(settingclock){
  case 1:         //Hour
    if(flicker)
      lcd_cleardigit(7);
    break;
  case 2:
    if(flicker)
      lcd_cleardigit(6);
    break;
  case 3:         //Minute
    if(flicker)
      lcd_cleardigit(4);
    break;
  case 4:
    if(flicker)
      lcd_cleardigit(3);
    break;
    
  case 5:        //Second
    /* We no longer set the seconds, but rather hold them at zero
       until the user moves back them into the date.  Mechanical watch
       experts call this 'hacking.'
     */
    
  case 7:        //Year
    if(flicker)
      lcd_cleardigit(7);
    break;
  case 8:
    if(flicker)
      lcd_cleardigit(6);
    break;
  case 9:
    if(flicker)
      lcd_cleardigit(5);
    break;
  case 10:
    if(flicker)
      lcd_cleardigit(4);
    break;
    
  case 11:        //Month
    if(flicker)
      lcd_cleardigit(3);
    break;
  case 12:
    if(flicker)
      lcd_cleardigit(2);
    break;
    
  case 13:       //Day
    if(flicker) 
      lcd_cleardigit(1);
    break;
  case 14:
    if(flicker)
      lcd_cleardigit(0);
    break;
  }
}




//! Exit clock when the sidebutton is pressed, unless we are programming.
int clock_exit(){
  if(settingclock){
    //Setting the time, so jump to next digit.
    settingclock++;
    return 1;
  }else{
    //Not setting the time, so just move on to next app.
    return 0;
  }
}

//! Draws the clock face in the main application.
void clock_draw(){
  /* Do *NOT* put anything here that takes a long time to calculate.
     Instead, do that in clock_keypress() once for the keydown event,
     and then show the result here.
  */
  
  static char ch=0;
  
  if(ch!=lastchar){
    lcd_zero();
    redraw++;
  }
  
  ch=lastchar;

  /* The SET button will move us into the programming mode. */
  if(sidebutton_set()){
    //Wait for the button to be released.
    while(sidebutton_set());
    //Then change invert the setting.
    settingclock=!settingclock;
  }

  if(settingclock)
    draw_settingtime();
  else
    switch(ch){
    case '7':
      //Hold 7 to run the self-test after startup.  Response codes try to
      //roughly describe the fault.
      post();
      break;
    case '8':
      //8 shows the callsign.
      lcd_string("        ");
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
    case '*':
      //* shows the chip model number.
      lcd_string(DEVICEID==DEVICEID6137?"430F6137":"430F6147");
      break;

    case '1':
      //1 shows the voltage.
      lcd_number(vcc);
      lcd_string("volt ");
      setperiod(2,1);
      break;
      
    case '0':
      //0 shows the name of the working channel.
      lcd_string(codeplug_name());
      break;
    case '.':
      //. shows the frequency of the working channel.
      lcd_number(codeplug_getfreq()/10);
      setperiod(5,1);
      setperiod(2,1);
      break;
    case '6':
    case 0:
      // Draw the time by default.
      draw_time(redraw);
      redraw=0;
      break;
    default:
      lcd_hex(ch);
    }

}


//! Entry to the clock app.
void clock_init(){
  lastchar=0;
  lcd_zero();
  draw_time(1);
}

//! A button has been pressed for the clock.
int clock_keypress(char ch){
  unsigned char inputdigit=0;
  lastchar=ch;

  /* This function is called *once* per keypress event, while the
     handlers in clock_draw() are called once per frame.  It is very
     important that anything that takes more than a frame be handled
     here, rather than in clock_draw(), which is rerun for every frame.
   */
  if(settingclock){
    //We only handle numbers here.
    if((ch&0x30)==0x30)
      inputdigit=ch&0x0F;
    else
      return 1;
    
    switch(settingclock){
    case 1:         //Hour
      SetRTCHOUR(inputdigit*10+RTCHOUR%10);
      settingclock++;
      break;
    case 2:
      SetRTCHOUR(RTCHOUR-RTCHOUR%10+inputdigit);
      settingclock++;
      break;
    case 3:         //Minute
      SetRTCMIN(inputdigit*10+RTCMIN%10);
      settingclock++;
      break;
    case 4:
      SetRTCMIN(RTCMIN-RTCMIN%10+inputdigit);
      settingclock=7;
      break;
    
      /* We no longer set the seconds, but rather hold them at zero
	 until the user moves back them into the date.  Mechanical watch
	 experts call this 'hacking.'
      */
    
    case 7:        //Year
      SetRTCYEAR(inputdigit*1000+RTCYEAR%1000);
      settingclock++;
      break;
    case 8:
      SetRTCYEAR(RTCYEAR-RTCYEAR%1000+inputdigit*100+RTCYEAR%100);
      settingclock++;
      break;
    case 9:
      SetRTCYEAR(RTCYEAR-RTCYEAR%100+inputdigit*10+RTCYEAR%10);
      settingclock++;
      break;
    case 10:
      SetRTCYEAR(RTCYEAR-RTCYEAR%10+inputdigit);
      settingclock++;
      break;
    
    case 11:        //Month
      SetRTCMON(inputdigit*10+RTCMON%10);
      settingclock++;
      break;
    case 12:
      SetRTCMON(RTCMON-RTCMON%10+inputdigit);
      settingclock++;
      break;
    
    case 13:       //Day
      SetRTCDAY(inputdigit*10+RTCDAY%10);
      settingclock++;
      break;
    case 14:
      SetRTCDAY(RTCDAY-RTCDAY%10+inputdigit);
      settingclock++;
      redraw++;
      ////Do not break, or we'll be stuck in the setting mode afer compltion.
      
    default:
      /* Once we've exceeded the count, it's time to return to the
	 normal mode.
      */
      settingclock=0;
    }

    //Update the DOW.  We could save some cycles by only doing this if
    //the date changes, but we don't bother.
    rtc_setdow();
  }else{
    switch(ch){
    case '1':
      /* To save on power, the reference is not active when idling.
       */
      ref_on();
      vcc=adc_getvcc();
      ref_off();
      break;
    case '6':
      //6 toggles the CPU load indicator.
      flickermode=(flickermode?0:-1);
      break;
    }
  }
  return 1;
}
