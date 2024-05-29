#ifndef VECTOR_H
#define VECTOR_H

#include <math.h>

// 3D Vector

struct Vector3 {
  float x;
  float y;
  float z;
};

float vector3_length(struct Vector3 vector) {
  return sqrtf( powf(vector.x, 2) + powf(vector.y, 2) + powf(vector.z, 2) );
}

struct Vector3 vector3_normalize(struct Vector3 vector) {
  struct Vector3 toRet;
  float len = vector3_length(vector);

  toRet.x = vector.x / len;
  toRet.y = vector.y / len;
  toRet.z = vector.z / len;

  return toRet;
}

struct Vector3 vector3_add(struct Vector3 one, struct Vector3 two) {
  struct Vector3 toRet;

  toRet.x = one.x + two.x;
  toRet.y = one.y + two.y;
  toRet.z = one.z + two.z;

  return toRet;
}

struct Vector3 vector3_multiply_scalar(struct Vector3 vector, float mult) {
  struct Vector3 toRet;

  toRet.x = vector.x * mult;
  toRet.y = vector.y * mult;
  toRet.z = vector.z * mult;

  return toRet;
}

struct Vector3 vector3_to_length(struct Vector3 vector, float len) {
  struct Vector3 normalized = vector3_normalize(vector);
  return vector3_multiply_scalar(normalized, len);
}

#endif //VECTOR_H
