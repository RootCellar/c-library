#ifndef COMPILER_ATTRIBUTES_H
#define COMPILER_ATTRIBUTES_H

/*
 * Compiler Version
*/

#ifdef __GNUC__
  #define COMPILER_SUPPORTS_GNU_C 1
#else
  #define COMPILER_SUPPORTS_GNU_C 0
#endif

#if COMPILER_SUPPORTS_GNU_C

  #define GNU_C_VERSION_IS_ATLEAST(major, minor, patch) \
      __GNUC__ > major || \
      (__GNUC__ == major && (__GNUC_MINOR__ > minor || \
                            (__GNUC_MINOR__ == minor && \
                             __GNUC_PATCHLEVEL__ >= patch)))

#else
  /* Some other compiler... */
 #define GNU_C_VERSION_IS_ATLEAST(major, minor, patch) (0)
#endif

/*
 * Inlining
*/

#if COMPILER_SUPPORTS_GNU_C
  #define COMPILER_ATTRIBUTE_ALWAYS_INLINE inline __attribute__ ((always_inline))
  #define COMPILER_ATTRIBUTE_NEVER_INLINE __attribute__ ((__noinline__))
  #define COMPILER_ATTRIBUTE_FLATTEN __attribute__ ((flatten))
#else
  #define COMPILER_ATTRIBUTE_ALWAYS_INLINE
  #define COMPILER_ATTRIBUTE_NEVER_INLINE
  #define COMPILER_ATTRIBUTE_FLATTEN
#endif

/*
 * Side affects
*/

#if GNU_C_VERSION_IS_ATLEAST(2, 96, 0)
  #define COMPILER_ATTRIBUTE_PURE __attribute__ ((pure))
#else
  #define COMPILER_ATTRIBUTE_PURE
#endif

#if GNU_C_VERSION_IS_ATLEAST(2, 5, 0)
  #define COMPILER_ATTRIBUTE_CONST __attribute__ ((const))
#else
  #define COMPILER_ATTRIBUTE_CONST
#endif

#endif
