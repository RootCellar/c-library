
#include <stdlib.h>

#define INT_SIZE (sizeof(int))
#define INTS_IN_BUFFER (2048)
#define DIE_BUFFER_SIZE (INTS_IN_BUFFER * INT_SIZE)

#define MAX_BUFFER_ROLL (0xF)
#define ROLLS_PER_INT (8)
#define BITS_PER_ROLL ((INT_SIZE*8)/ROLLS_PER_INT)

unsigned char die_buffer[DIE_BUFFER_SIZE];
int die_loc = DIE_BUFFER_SIZE;

int roll_die(int sides) {

  if(sides < MAX_BUFFER_ROLL) {
    if(die_loc >= DIE_BUFFER_SIZE) {
        for(int k = 0; k < DIE_BUFFER_SIZE; k += ROLLS_PER_INT) {
          int num = rand();
          for(int i = 0; i < ROLLS_PER_INT; i++) {
            die_buffer[k + i] = num & MAX_BUFFER_ROLL;
            num >>= BITS_PER_ROLL;
          }
        }
        die_loc = 0;
    }
    return ((int)die_buffer[die_loc++] % sides) + 1;
  }

  return (rand() % sides) + 1;
}

int roll_dice(int count, int sides) {
  int sum = 0;
  for(int i = 0; i < count; i++) {
    sum += roll_die(sides);
  }

  return sum;
}
