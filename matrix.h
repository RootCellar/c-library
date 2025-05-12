#ifndef MATRIX_H
#define MATRIX_H

size_t index_2d(size_t x, size_t y, size_t width);
size_t index_3d(size_t x, size_t y, size_t z, size_t width, size_t height);
size_t index_4d(size_t x, size_t y, size_t z, size_t w, size_t width, size_t height, size_t depth);

#endif
