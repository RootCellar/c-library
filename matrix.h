#ifndef MATRIX_H
#define MATRIX_H

size_t index_2d(int x, int y, int width) {
  return y * width + x;
}

size_t index_3d(int x, int y, int z, int width, int height) {
  return z * width * height + index_2d(x, y, width);
}

size_t index_4d(int x, int y, int z, int w, int width, int height, int depth) {
  return w * width * height * depth + index_3d(x, y, z, width, height);
}

#endif
