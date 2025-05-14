#ifndef VECTOR_H
#define VECTOR_H

// 3D Vector

struct Vector3 {
  float x;
  float y;
  float z;
};

struct Vector3 vector3_create(const float x, const float y, const float z);
float vector3_length(const struct Vector3 vector);
struct Vector3 vector3_normalize(const struct Vector3 vector);
struct Vector3 vector3_negate(const struct Vector3 vector);
struct Vector3 vector3_add(const struct Vector3 one, const struct Vector3 two);
struct Vector3 vector3_subtract(const struct Vector3 one, const struct Vector3 two);
struct Vector3 vector3_multiply_scalar(const struct Vector3 vector, const float mult);
struct Vector3 vector3_to_length(const struct Vector3 vector, const float len);
float vector3_dot_product(const struct Vector3 one, const struct Vector3 two);
struct Vector3 vector3_cross_product(const struct Vector3 one, const struct Vector3 two);
struct Vector3 vector3_facing(const struct Vector3 one, const struct Vector3 two);

#endif //VECTOR_H
