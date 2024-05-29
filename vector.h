#ifndef VECTOR_H
#define VECTOR_H

#include <math.h>

// 3D Vector

struct Vector3 {
  float x;
  float y;
  float z;
};

struct Vector3 vector3_create(float x, float y, float z) {
  struct Vector3 toRet;

  toRet.x = x;
  toRet.y = y;
  toRet.z = z;

  return toRet;
}

float vector3_length(struct Vector3 vector) {
  return sqrtf( powf(vector.x, 2) + powf(vector.y, 2) + powf(vector.z, 2) );
}

struct Vector3 vector3_normalize(struct Vector3 vector) {
  float len = vector3_length(vector);
  return vector3_create(vector.x / len, vector.y / len, vector.z / len);
}

struct Vector3 vector3_negate(struct Vector3 vector) {
  return vector3_create(vector.x * -1, vector.y * -1, vector.z * -1);
}

struct Vector3 vector3_add(struct Vector3 one, struct Vector3 two) {
  return vector3_create(one.x + two.x, one.y + two.y, one.z + two.z);
}

struct Vector3 vector3_subtract(struct Vector3 one, struct Vector3 two) {
  return vector3_add( one, vector3_negate(two) );
}

struct Vector3 vector3_multiply_scalar(struct Vector3 vector, float mult) {
  return vector3_create(vector.x * mult, vector.y * mult, vector.z * mult);
}

struct Vector3 vector3_to_length(struct Vector3 vector, float len) {
  struct Vector3 normalized = vector3_normalize(vector);
  return vector3_multiply_scalar(normalized, len);
}

float vector3_dot_product(struct Vector3 one, struct Vector3 two) {
  return one.x * two.x + one.y * two.y + one.z * two.z;
}

struct Vector3 vector3_cross_product(struct Vector3 one, struct Vector3 two) {
  return vector3_create(
    one.y * two.z - one.z * two.y,
    one.z * two.x - one.x * two.z,
    one.x * two.y - one.y * two.x
  );
}

#endif //VECTOR_H
