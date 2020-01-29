#include<stdio.h>
#include<string.h>
#include<msp430.h>
#include "api.h"


//TODO: optimize by doing it in place with an already 2*data_size array
static unsigned char* manchester_encode(unsigned char *data, size_t size){
    unsigned int i;
    unsigned char * out = malloc(2*size);
    unsigned int j;
    unsigned char v;
    char bit;
    for(i=0 ; i<size ; i++){
        //vl
        v = 0;
        for(j=0 ; j<4 ; j++){
            bit = ((data[i] << (7-j)) & 255) >> 7;
            v += (2 ^ (bit << 1 | bit)) << (2*j);
        }
        out[2*i+1] = v;
        //vh
        v = 0;
        for(j=4 ; j<8 ; j++){
            bit = ((data[i] << (7-j)) & 255) >> 7;
            v += (2 ^ (bit << 1 | bit)) << (2*(j-4));
        }
        out[2*i] = v;
    }
    return out;
}


