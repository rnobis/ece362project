#include <stdio.h>
#include <stdint.h>

/*Code for an 8-bit Linear feedback shift register (lfsr)
  needs to be implemented to the uC, can be done in C or asm
  */

int main(){
  uint8_t = lfsr = 0xAC;
  unsigned bit;
  unsigned period = 0;
  int i = 0;
  
    do{
      bit = ((lfsr >> 0) ^ (lfsr >> 1) ^ (lfsr >> 2) ^ (lfsr >> 4)) & 1;
      lfsr = (lfsr >> 1) | (bit << 7);
      printf("%d: %d %d\n", i, bit, lfsr % 4);
      i += 1;
    }while(1);
    
    return 0;
}
