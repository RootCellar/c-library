
#include "test.h"
#include "statistics.h"

void main() {

  SECTION("Statistics");

  float two_values[] = {0.0, 10.0};
  TEST( floats_equal( statistics_average(two_values, 2), 5.0 ), "average of two values" );

  float many_values[] = {0.0, 10.0, 5.0, 2.5, 7.5, -5.0, 15.0};
  TEST( floats_equal( statistics_average(many_values, 7), 5.0 ), "average of many values" );
  
}
