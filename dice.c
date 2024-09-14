#include <stdio.h>
#include <time.h>

#include "random.h"

void show_usage(void) {
  printf("Usage: dice [count] <sides>\n");
}

int main(int argc, char** argv) {

  srand(time(NULL));

  int sides;
  int count = 1;

  if(argc < 2) {
    show_usage();
    exit(1);
  } else if(argc == 2) {
    if(sscanf(argv[1], "%d", &sides) != 1) {
      show_usage();
      exit(1);
    }
  } else if(argc == 3) {
    if(sscanf(argv[1], "%d", &count) != 1 || sscanf(argv[2], "%d", &sides) != 1) {
      show_usage();
      exit(1);
    }
  } else {
    show_usage();
    exit(1);
  }

  if(count < 1 || sides < 1) {
    show_usage();
    exit(1);
  }

  printf("%d\n", roll_dice(count, sides));
  exit(0);
}
