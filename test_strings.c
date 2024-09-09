#include <stdio.h>

#include "test.h"
#include "strings.h"

void main() {

  char buf[128];

  SECTION("Digits");

  for(int i = 0; i < 10; i++) {
    char c = '0' + i;
    sprintf(buf, "%c is a digit", c);
    TEST(is_digit(c), buf);
  }

  exit(0);
}
