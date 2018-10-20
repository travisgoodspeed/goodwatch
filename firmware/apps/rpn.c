/*! \file rpn.c
   \brief RPN Calculator.

   This is a simple RPN calculator, so that our calculator watch can
   still calculate.  The stack is 10 longs deep, and you should press
   the side button to push the current value into the stack.  '=' also
   pushes, and the '.' key is not yet supported.
   
   The AM pixel indicates that the current value has not yet been
   pushed to the stack.
   
   
*/


#include "api.h"
#include "rpn.h"

#define STACKSIZE 10

static long stack[STACKSIZE];
static long buffer=0;
static int bufferdirty=0;
static unsigned int stacki=0;


//! Peeks at the top item of the stack.
static long rpn_peek(){
  return stack[stacki%STACKSIZE];
}
//! Pushes a new value onto the stack.
static void rpn_push(long val){
  stack[(++stacki)%STACKSIZE]=val;
}
//! Pops the top item from the stack.
static long rpn_pop(){
  return stack[(stacki--)%STACKSIZE];
}



//! Draws the top of the RPN stack.
static void rpn_drawstack(){
  long todraw=0;
  
  //AM indicates the current input.
  setam(bufferdirty?1:0);
  
  if(!bufferdirty)
    todraw=stack[stacki%STACKSIZE];
  else
    todraw=buffer;

  lcd_number(todraw);
}

//! Pushes the buffer to the stack if it has been modified.
static void rpn_pushbuffer(){
  //Do nothing unless the buffer is in use.
  if(!bufferdirty) return;

  //If the buffer is dirty, we'll zero it, mark it clean, and push it
  //to the call stack.
  rpn_push(buffer);
  buffer=0;
  bufferdirty=0;
}
//! Presses one digit into the buffer.
static void rpn_updatebuffer(int i){
  /* Here we mark the buffer as dirty, multiple it by ten, and add the
     incoming digit.  This would of course be more efficient in BCD,
     but GCC doesn't support BCD values and we'd prefer not to write
     in assembly for now.
   */
  
  bufferdirty=1;
  buffer=buffer*10+i;
}


//! Initializes the calculator.
void rpn_init(){
  int i;
  
  //Fresh stack when we enter the calculator.
  for(i=0;i<STACKSIZE;i++)
    rpn_push(0);

  //Force the first draw.
  rpn_draw(1);
}

//! RPN handler for sidebutton.
int rpn_exit(){
  if(bufferdirty){
    // Push the incoming number if it's not yet committed.
    rpn_pushbuffer();
    rpn_draw(1); // Redraw screen to clear SetAM when side button used
    return 1;
    
  } else if(rpn_peek()==0){
    // Exit if zero is the latest number.
    return 0;
  }
  
  //Otherwise push a zero and return.
  rpn_push(0);
  return 1;
}

//! Draws the RPN calculator.
void rpn_draw(int forced){
  //We only draw if forced to.
  if(forced)
    rpn_drawstack();
}

//! A button has been pressed for the calculator.
int rpn_keypress(char ch){
  long i, j;

  //Do nothing on a keyup event.
  if(!ch)
    return 0;


  //Operators
  switch(ch){
  case '=':
    if(bufferdirty)
      rpn_pushbuffer();
    else
      rpn_push(rpn_peek());
    break;
  case '.':
    //What should this do?
    //It's our only free button.
    break;
  case '+':
    rpn_pushbuffer();
    rpn_push(rpn_pop()+rpn_pop());
    break;
  case '-':
    rpn_pushbuffer();
    j=rpn_pop();
    i=rpn_pop();
    rpn_push(i-j);
    break;
  case '*':
    rpn_pushbuffer();
    rpn_push(rpn_pop()*rpn_pop());
    break;
  case '/':
    rpn_pushbuffer();
    j=rpn_pop();
    i=rpn_pop();
    rpn_push(i/j);
    break;
  }

  
  /* Numbers are special.  They modify a buffer, and the buffer is
     pushed onto the stack before an operator or when = is pressed.
  */
  if(ch>='0' && ch<='9')
    rpn_updatebuffer(ch&0xf);

  return 1;//Redraw.
}
