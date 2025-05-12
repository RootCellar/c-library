#include <stddef.h>

#include "matrix.h"

size_t index_2d(size_t x, size_t y, size_t width) {
  return y * width + x;
}

size_t index_3d(size_t x, size_t y, size_t z, size_t width, size_t height) {
  return z * width * height + index_2d(x, y, width);
}

size_t index_4d(size_t x, size_t y, size_t z, size_t w, size_t width, size_t height, size_t depth) {
  return w * width * height * depth + index_3d(x, y, z, width, height);
}
