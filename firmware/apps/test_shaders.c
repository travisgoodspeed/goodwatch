#include <stdlib.h>
#include <stdlib.h>

typedef unsigned char uint8_t;

static void manchester_encode(uint8_t *data, size_t size){
  unsigned int starting_pos = size/2;
  unsigned int i;
  unsigned int j;
  uint8_t v;
  uint8_t bit;
  for(i=starting_pos ; i<size ; i++){
    //vh
    v = 0;
    for(j=4 ; j<8 ; j++){
      bit = ((data[i] << (7-j)) & 255) >> 7;
      v += (2 ^ (bit << 1 | bit)) << (2*(j-4));
    }
    //vl
    data[2*(i-starting_pos)] = v;
    v = 0;
    for(j=0 ; j<4 ; j++){
      bit = ((data[i] << (7-j)) & 255) >> 7;
      v += (2 ^ (bit << 1 | bit)) << (2*j);
    }
    data[2*(i-starting_pos)+1] = v;
  }
}


int test_manchester_encode(){
    uint8_t data[14];
    memset(data, 0, 14);
    data[7] =  0b11111111;
    data[8] =  0b00000000;
    data[9] =  0b10101010;
    data[10] = 0b00110011;
    data[11] = 0;
    data[12] = 0;
    data[13] = 0b10101010;

    uint8_t expected[14] = {0b01010101, 0b01010101,
                            0b10101010, 0b10101010,
                            0b01100110, 0b01100110,
                            0b10100101, 0b10100101,
                            0b10101010, 0b10101010,
                            0b10101010, 0b10101010,
                            0b01100110, 0b01100110};

    manchester_encode(data, 14);
    for (int i=0 ; i<14 ; i++){
        if (expected[i] != data[i]){
            printf("error for index %d. data[%i]=%x\n", i, i, data[i]);
            return -1;
        }
    }
    return 0;
}


int test_encrypt(){
    uint8_t data[7];
    memset(data, 0, 7);
    data[0] = 0xaa;
    data[1] = 0b00000000;
    data[2] = 0b10101010;
    data[3] = 0b00110011;
    data[4] = 0;
    data[5] = 0;
    data[6] = 0b10101010;

    uint8_t expected[14] = {0b01010101, 0b01010101,
                            0b10101010, 0b10101010,
                            0b01100110, 0b01100110,
                            0b10100101, 0b10100101,
                            0b10101010, 0b10101010,
                            0b10101010, 0b10101010,
                            0b01100110, 0b01100110};
}

int main(){
    test_manchester_encode();
    return 0;
}