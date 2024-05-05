#include "test.h"
#include "memory.h"

void main() {
  TEST( tResize(-1) == 1, "tResize(-1)");
  TEST( tResize(0) == 1, "tResize(0)");

  TEST( tResize(1) == 0, "tResize(1)");
  TEST( tResize(1) == 0, "tResize(1) again");

  TEST( tMalloc(-1) == NULL, "tMalloc(-1)");
  TEST( tMalloc(0) == NULL, "tMalloc(0)");
}
