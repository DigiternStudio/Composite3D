#ifndef CM3D_STD_TYPES_HPP
#define CM3D_STD_TYPES_HPP

#include <cstdint>
#include <cstddef>

typedef uint8_t sByte;
typedef int64_t sSize;
typedef double sReal;

#if HAS_LONG_DOUBLE
  typedef long double sLongReal;
#else
  typedef double sLongReal;
#endif

#endif