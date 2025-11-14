#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "../lib/time.h"
#include "../lib/random.h"

void show_usage(void) {
  printf("Usage: dice [[s[s]]count] <sides>\n");
  exit(1);
}

int main(int argc, char** argv) {

  srand(get_time().tv_nsec);

  int sides;
  int count = 1;

  int separate = 0;
  int show_sum = 0;

  if(argc < 2) {
    show_usage();
  } else if(argc == 2) {
    if(sscanf(argv[1], "%d", &sides) != 1) {
      show_usage();
    }
  } else if(argc == 3) {
    if(sscanf(argv[1], "%d", &count) == 1) {

    } else if(sscanf(argv[1], "s%d", &count) == 1) {
        separate = 1;
    } else if(sscanf(argv[1], "ss%d", &count) == 1) {
        show_sum = separate = 1;
    } else {
        show_usage();
    }

    if(sscanf(argv[2], "%d", &sides) != 1) {
      show_usage();
    }
  } else {
    show_usage();
  }

  if(count < 1 || sides < 1) {
    show_usage();
  }

  if(!separate) printf("%d\n", roll_dice(count, sides));
  else {
    int sum = 0;
    for(int i = 0; i < count; i++) {
      int roll = roll_die(sides);
      sum += roll;
      printf("%d ", roll);
      if(!isatty(STDOUT_FILENO) && i < count-1) printf("\n");
    }
    printf("\n");
    if(show_sum) printf("%d\n", sum);
  }
  exit(0);
}
