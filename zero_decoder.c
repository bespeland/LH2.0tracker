#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define PBIN "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c"
#define I2B(b) \
  (b & 0x80000000 ? '1' : '0'), \
  (b & 0x40000000 ? '1' : '0'), \
  (b & 0x20000000 ? '1' : '0'), \
  (b & 0x10000000 ? '1' : '0'), \
  (b & 0x08000000 ? '1' : '0'), \
  (b & 0x04000000 ? '1' : '0'), \
  (b & 0x02000000 ? '1' : '0'), \
  (b & 0x01000000 ? '1' : '0'), \
  (b & 0x800000 ? '1' : '0'), \
  (b & 0x400000 ? '1' : '0'), \
  (b & 0x200000 ? '1' : '0'), \
  (b & 0x100000 ? '1' : '0'), \
  (b & 0x080000 ? '1' : '0'), \
  (b & 0x040000 ? '1' : '0'), \
  (b & 0x020000 ? '1' : '0'), \
  (b & 0x010000 ? '1' : '0'), \
  (b & 0x8000 ? '1' : '0'), \
  (b & 0x4000 ? '1' : '0'), \
  (b & 0x2000 ? '1' : '0'), \
  (b & 0x1000 ? '1' : '0'), \
  (b & 0x0800 ? '1' : '0'), \
  (b & 0x0400 ? '1' : '0'), \
  (b & 0x0200 ? '1' : '0'), \
  (b & 0x0100 ? '1' : '0'), \
  (b & 0x80 ? '1' : '0'), \
  (b & 0x40 ? '1' : '0'), \
  (b & 0x20 ? '1' : '0'), \
  (b & 0x10 ? '1' : '0'), \
  (b & 0x08 ? '1' : '0'), \
  (b & 0x04 ? '1' : '0'), \
  (b & 0x02 ? '1' : '0'), \
  (b & 0x01 ? '1' : '0')

uint32_t polynomials[35] =
        {
          // 0x00000011,
             // x^17 + x^13 + x^12 + x^10 + x^7 + x^4 + x^2 + x^1 + 1
             //   0123456789ABCDEF01
             // 0b11101001001011000
            0x0001D258,
            // x^17 + x^14 +  x^7 +  x^6 + x^5 + x^4 + x^3 + x^2 + 1
            //  0123456789ABCDEF01
            //0b10111111000000100
            0x00017E04,
            0x0001FF6B, 0x00013F67,
            0x0001B9EE, 0x000198D1,
            // x^17 + x^16 + x^14 + x^12 + x^10 + x^7 + x^5 + x^1 + 1
            // 0b11000101001010101
            // 0b11000101001010101
            0x000178C7, 0x00018A55,
            0x00015777, 0x0001D911,
            0x00015769, 0x0001991F,
            0x00012BD0, 0x0001CF73,
            0x0001365D, 0x000197F5,
            0x000194A0, 0x0001B279,
            0x00013A34, 0x0001AE41,
            0x000180D4, 0x00017891,
            0x00012E64, 0x00017C72,
            0x00019C6D, 0x00013F32,
            0x0001AE14, 0x00014E76,
            0x00013C97, 0x000130CB,
            0x00013750, 0x0001CB8D,
            0x000D3675, 0x00090C2D,
            0x000AE6CB,
};

#define INPUT_BIN 2
#define INPUT_DEC 10
#define INPUT_HEX 16

int bit(int b, int n){
  return (b >> n)&1;
}


int main(int argc, char** argv){
  if(argc < 2){
    printf("Add BMC decoded bitsequence arg\n");
    return 0;
  }
  unsigned long bits;
  if(argc > 2){
    bits = strtoul(argv[1], NULL, atoi(argv[2]));
  } else {
    bits = strtoul(argv[1], NULL, INPUT_HEX);
  }
  printf("%lu\n", bits);
  // return 0;

  // bits = cf1e86e3;

  int num_polys = sizeof(polynomials)/sizeof(polynomials[0]);

  int zeros[35];

  //find matches
  for(int poly_i = 0; poly_i < num_polys; poly_i++ ){
    zeros[poly_i] = 0;
    //run for each polynomial
    uint32_t poly = polynomials[poly_i];
    printf("---  Testing poly%d:0x%x  ---\n", poly_i,poly);


    // printf(PBIN"-\n", I2B(bits));
    for(int j = 0; j < (32-20); j++){
      // printf(PBIN"\n", I2B(bits>>j));
      unsigned int b = (bits>>j)&poly;
      // printf(PBIN"\n", I2B(b));
      b^=b>>16;
      // printf(PBIN"\n", I2B(b));
      b^=b>>8;
      // printf(PBIN"\n", I2B(b));
      b^=b>>4;
      // printf(PBIN"\n", I2B(b));
      b^=b>>2;
      // printf(PBIN"\n", I2B(b));
      b^=b>>1;
      // printf(PBIN"\n", I2B(b));
      b&=1; // parity
      // printf(PBIN"\n", I2B(b));
      // printf("---\n");
      if (!b){
        // printf("%x,\n", b);
        zeros[poly_i]++; // count zeroes
      }
    }

    printf("%d\n",zeros[poly_i]);



  }

  int max = 0;
  int maxi = 0;

  for(int i = 0; i < num_polys; i++){
    if(zeros[i] > max){
      max = zeros[i];
      maxi = i;
    }
  }

  printf("Max zeros of %d at poly%d: %d\n", max,maxi, polynomials[maxi]);








}
//11010110100101101010001010110101 clean
//11010110100101101010011010101011 noisy
//0xD696A6AB