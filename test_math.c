
#include <time.h>

#include "test.h"
#include "debug.h"
#include "vector.h"

void main() {

  srand(time(NULL));

  SECTION("3D Vectors");

  SECTION("Basic 3D Vector Checks");

  struct Vector3 vector, vector2;

  vector.x = 1;
  vector.y = 1;
  vector.z = 1;

  TEST( floats_equal( vector3_length(vector), 1.732050807568f ), "length of (1,1,1)" );

  vector2.x = 3;
  vector2.y = 3;
  vector2.z = 3;

  vector = vector3_add(vector, vector2);

  TEST( floats_equal(vector.x, 4.0f), "vector.x after add" );
  TEST( floats_equal(vector.y, 4.0f), "vector.y after add" );
  TEST( floats_equal(vector.z, 4.0f), "vector.z after add" );

  TEST( floats_equal( vector3_length(vector), 6.928203230f ), "length of (4,4,4)" );

  vector = vector3_normalize(vector);

  TEST( floats_equal( vector3_length(vector), 1.0f ), "length after normalize" );

  SECTION("Many 3D Vector Operations");

  int num_tests = 2000000;

  for(int i = 0; i < num_tests; i++) {
    float len = rand() % 100 + 0.01 * (float) (rand() % 100);
    vector.x = rand() % 1000;
    vector.y = rand() % 1000;
    vector.z = rand() % 1000;

    __TEST( floats_equal( vector3_length( vector3_normalize(vector) ), 1.0f ), "vector3_normalize", 1);
    __TEST( floats_equal( vector3_length( vector3_to_length(vector, len) ), len ), "vector3_to_length", 1);
  }

  exit(0);

}