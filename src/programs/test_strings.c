#include <stdio.h>

#include "../lib/unit_testing.h"
#include "../lib/strings.h"

void main() {

  char buf[128];

  SECTION("Digits");

  for(int i = 0; i < 10; i++) {
    unsigned char c = '0' + i;
    sprintf(buf, "%c is a digit", c);
    TEST(is_digit(c), buf);
  }

  for(int i = 0; i < 256; i++) {
    if(i >= '0' && i <= '9') continue;
    unsigned char c = i;
    sprintf(buf, "ASCII Code %d is NOT a digit", c);
    TEST(is_digit(c) == 0, buf);
  }

  SHOW_TEST_RESULTS();

  exit(0);
}
