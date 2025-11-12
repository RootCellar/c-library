#ifndef COMPILER_ATTRIBUTES_H
#define COMPILER_ATTRIBUTES_H

#define COMPILER_ATTRIBUTE_ALWAYS_INLINE inline __attribute__((always_inline))
#define COMPILER_ATTRIBUTE_NEVER_INLINE __attribute__((__noinline__))
#define COMPILER_ATTRIBUTE_FLATTEN __attribute__((flatten))

#endif