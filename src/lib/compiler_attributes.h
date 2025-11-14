#ifndef COMPILER_ATTRIBUTES_H
#define COMPILER_ATTRIBUTES_H

/*
 * Compiler Version
*/

#define GCC_VERSION_IS_ATLEAST(major, minor, patch) \
    __GNUC__ > major || \
    (__GNUC__ == major && (__GNUC_MINOR__ > minor || \
                          (__GNUC_MINOR__ == minor && \
                           __GNUC_PATCHLEVEL__ >= patch)))

#define GCC_VERSION_IS_EARLIER_THAN(major, minor, patch) \
    __GNUC__ < major || \
    (__GNUC__ == major && (__GNUC_MINOR__ < minor || \
                          (__GNUC_MINOR__ == minor && \
                           __GNUC_PATCHLEVEL__ < patch)))

/*
 * Inlining
*/

#define COMPILER_ATTRIBUTE_ALWAYS_INLINE inline __attribute__ ((always_inline))
#define COMPILER_ATTRIBUTE_NEVER_INLINE __attribute__ ((__noinline__))
#define COMPILER_ATTRIBUTE_FLATTEN __attribute__ ((flatten))

/*
 * Side affects
*/

#if GCC_VERSION_IS_ATLEAST(2, 96, 0)
  #define COMPILER_ATTRIBUTE_PURE __attribute__ ((pure))
#else
  #define COMPILER_ATTRIBUTE_PURE
  #warning "PURE function attribute not supported."
#endif

#if GCC_VERSION_IS_ATLEAST(2, 5, 0)
  #define COMPILER_ATTRIBUTE_CONST __attribute__ ((const))
#else
  #define COMPILER_ATTRIBUTE_CONST
  #warning "CONST function attribute not supported."
#endif

#endif