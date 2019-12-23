#pragma once

#if __WORDSIZE == 64
#  define MIs64Bit
#endif

#define protecting(var, body) \
  auto old##var = (var); \
  (body); (var) = old##var;

template <typename T>
void *voidPtr(T v) { return static_cast<void *>(v); }

typedef const char *CString;
const char EOT = '\x04';
