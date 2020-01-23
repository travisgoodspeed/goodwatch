/*! \file submenu.c
  \brief Submenu selection tool.

  
  This little applet lists a submenu of applications, so that the user
  can choose between many rarely used applets without having to wade
  through an endless ring of of them.
  
  Press + and - to choose between the applets, and the side button to
  move into the chosen applet.
  
*/

#include "api.h"
#include "applist.h"

//! Index of the applet in the submenu.
static unsigned int subindex=0;

//! Redraw the submenu.
static void redraw(){
  lcd_string("        ");
  lcd_string(subapps[subindex].name);
}

//! Draw the submenu selection.
void submenu_draw(){
  static unsigned int lastindex=0;

  //Redraw only when the index changes.
  if(lastindex!=subindex)
    redraw();
  lastindex=subindex;
}

//! Enter the submenu.
void submenu_init(){
  // indicate that we're in the submenu by setting the plus and minus digits
  setplus(1); 
  setminus(1);
  redraw();
}

//! Change the selected applet.
int submenu_keypress(char c){
  switch(c){
  case '+':
    subindex++;
    if(!subapps[subindex].name)
      subindex=0;
    break;
  case '-':
    if(subindex)
      subindex--;
    else{
      while(subapps[++subindex+1].name);
    }
    break;
  }

  return 1;//Redraw.
}

//! On exit, set the submenu app.
int submenu_exit(){
  // unset the signs on submenu exit
  setplus(0); 
  setminus(0);
  //Set the new app.
  app_set(&subapps[subindex]);
  //Return 1 so app_next() won't move us to the next major app.
  return 1;
}

//! Lets a keypress fall through from the clock to the select submenu applet.
int submenu_fallthrough(char ch){
  /* Technically any key which is not handled by the clock applet will
     fall through, but only the third row (1,2,3,-) is expected to
     remain unused.
   */
  if(subapps[subindex].fallthrough)
    return subapps[subindex].fallthrough(ch);

  //Return 1 if there is no handler, so tha the screen is redrawn.
  return 1;
}
