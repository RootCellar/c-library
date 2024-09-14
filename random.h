#ifndef RAND_H
#define RAND_H

#include <stdlib.h>

int roll_die(int sides) {
  return (rand() % sides) + 1;
}

int roll_dice(int count, int sides) {
  int sum = 0;
  for(int i = 0; i < count; i++) {
    sum += roll_die(sides);
  }

  return sum;
}

#endif
