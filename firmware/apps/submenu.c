/*! \file submenu.c
  \brief Submenu selection tool.
*/

#include "api.h"
#include "applist.h"

//! Index of the applet in the submenu.
static unsigned int subindex=0;

//! Draw the submenu selection.
void submenu_draw(){
  // indicate that we're in the submenu by setting the plus and minus digits
  setplus(1); 
  setminus(1);
  lcd_string("        ");
  lcd_string(subapps[subindex].name);
}

//! Change the selected applet.
void submenu_keypress(char c){
  switch(c){
  case '+':
    subindex++;
    if(!subapps[subindex].name)
      subindex=0;
    break;
  case '-':
    if(subindex)
      subindex--;
    break;
  }
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

//! Draw the submenu selected.
void submenu_drawselected(){
  lcd_string("selected");
}
