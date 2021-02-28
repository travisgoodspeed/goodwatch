#include "phonebook.h"

#ifndef STANDALONE
#include "api.h"
#endif

//Fake phonebook only in standalone mode.
#ifdef STANDALONE
#define PHONEBOOK \
  "Travis  " "555-3811" "\n" \
  "bob     " "555-0321" "\n" \
  "Jim     " "555-9379" "\n" \
  "Last    " "555-2358" "\n"



#include<stdint.h>
#include<stdio.h>
#include<string.h>

int main(){
  const char* p=phonebook;
  char w[9]="        ";
  for(int i=0; i<10; i++){
    for(int j=0; j<10; j++){
      memcpy(w,p,8);
      printf("\t_%s_",w);
      p=pb_nextword(p);
    }
    printf("\n");
    p=pb_nextline(p);
  }
  return 0;
}

#endif



//! This is variable for the phonebook.
const char phonebook[] = PHONEBOOK;



//! Given a pointer, this goes back to the beginning of the line.
const char* pb_firstword(const char* word){
  while(word-->phonebook){
    //One step forward from a newline.
    if(*word=='\n')
      return word+1;
  }

  return phonebook;
}


//! Given a pointer, it finds the very next word while looping to the first of a line.
const char* pb_nextword(const char* word){
  //Step forward eight letters, unless we find a terminator.
  for(int i=0; i<8; i++){
    word++;

    //On a null or newline, jump back to the beginning of the line.
    if(!*word || *word=='\n')
      return pb_firstword(word);
  }

  //One more to check.
  word++;
  switch(*word){
  case '\n':
  case 0:
    //If a newline or null, we jump back to the start.
    return pb_firstword(--word);
  default:
    return --word;
  }
}


//! Given a pointer, this finds the next line.
const char* pb_nextline(const char* word){
  while(phonebook < ++word +sizeof(phonebook)){
    //Revert to the first line on a null.
    if(!*word)
      return phonebook;


    //A newline is either preceeded by more text or by a null.
    if(*word=='\n'){
      word++;
      
      if(*word)//Return the text.
	return word;
      else//Revert to the first line on a null.
	return phonebook;
    }
  }

  //Jump back to the beginning when we go too far.
  return phonebook;
}
