
#include "test.h"
#include "statistics.h"

void main() {

  SECTION("Statistics");

  float two_values[] = {0.0, 10.0};
  TEST( floats_equal( statistics_average(two_values, 2), 5.0 ), "average of two values" );
  TEST( floats_equal( statistics_average_deviation(two_values, 2), 5.0 ), "average deviation of two values" );
  TEST( floats_equal( statistics_standard_deviation(two_values, 2), 5.0 ), "standard deviation of two values" );

  float many_values[] = {0.0, 10.0, 5.0, 2.5, 7.5, -5.0, 15.0};
  TEST( floats_equal( statistics_average(many_values, 7), 5.0 ), "average of many values" );
  TEST( floats_equal( statistics_average_deviation(many_values, 7), 5.0 ), "statistics_average_deviation of many values" );
  TEST( floats_equal( statistics_standard_deviation(many_values, 7), 6.1237243569579 ), "statistics_standard_deviation of many values" );
  
}
