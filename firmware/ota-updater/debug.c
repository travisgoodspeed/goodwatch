#include <stdint.h>
#include "debug.h"

#ifdef DEBUG
static char *debug = (char *) DEBUG_LOG_ADDRESS;

void LC(char c){
	*debug++ = c;
}

void L(char *msg){
	while(*msg)
		LC(*msg++);
	LC('\n');
}

#else
void LC(char c){};
void L(char *msg){};
#endif
