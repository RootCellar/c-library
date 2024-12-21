#ifndef VECTOR_H
#define VECTOR_H

#include <math.h>

// 3D Vector

struct Vector3 {
  float x;
  float y;
  float z;
};

struct Vector3 vector3_create(const float x, const float y, const float z) {
  return (struct Vector3) {.x = x, .y = y, .z = z};
}

float vector3_length(const struct Vector3 vector) {
  return sqrtf(powf(vector.x, 2) + powf(vector.y, 2) + powf(vector.z, 2));
}

struct Vector3 vector3_normalize(const struct Vector3 vector) {
  float len = vector3_length(vector);
  return vector3_create(vector.x / len, vector.y / len, vector.z / len);
}

struct Vector3 vector3_negate(const struct Vector3 vector) {
  return vector3_create(vector.x * -1, vector.y * -1, vector.z * -1);
}

struct Vector3 vector3_add(const struct Vector3 one, const struct Vector3 two) {
  return vector3_create(one.x + two.x, one.y + two.y, one.z + two.z);
}

struct Vector3 vector3_subtract(const struct Vector3 one, const struct Vector3 two) {
  return vector3_create(one.x - two.x, one.y - two.y, one.z - two.z);
}

struct Vector3 vector3_multiply_scalar(const struct Vector3 vector, const float mult) {
  return vector3_create(vector.x * mult, vector.y * mult, vector.z * mult);
}

struct Vector3 vector3_to_length(const struct Vector3 vector, const float len) {
  float factor = len / vector3_length(vector);
  return vector3_multiply_scalar(vector, factor);
}

float vector3_dot_product(const struct Vector3 one, const struct Vector3 two) {
  return one.x * two.x + one.y * two.y + one.z * two.z;
}

struct Vector3 vector3_cross_product(const struct Vector3 one, const struct Vector3 two) {
  return vector3_create(
    one.y * two.z - one.z * two.y,
    one.z * two.x - one.x * two.z,
    one.x * two.y - one.y * two.x
  );
}

struct Vector3 vector3_facing(const struct Vector3 one, const struct Vector3 two) {
  return vector3_normalize(vector3_subtract(two, one));
}

#endif //VECTOR_H
