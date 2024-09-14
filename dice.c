#include <stdio.h>
#include <time.h>

#include "random.h"

void show_usage(void) {
  printf("Usage: dice [count] <sides>\n");
  exit(1);
}

int main(int argc, char** argv) {

  srand(time(NULL));

  int sides;
  int count = 1;

  int separate = 0;

  if(argc < 2) {
    show_usage();
  } else if(argc == 2) {
    if(sscanf(argv[1], "%d", &sides) != 1) {
      show_usage();
    }
  } else if(argc == 3) {
    if(sscanf(argv[1], "%d", &count) != 1) {
      separate = 1;
      if(sscanf(argv[1], "s%d", &count) != 1) {
        show_usage();
      }
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
    for(int i = 0; i < count; i++) {
      printf("%d ", roll_die(sides));
    }
    printf("\n");
  }
  exit(0);
}
