// cp-mac: GCC/libstdc++ extensions that CP code relies on, recreated for
// libc++ (Apple Clang, Homebrew LLVM, clang -stdlib=libc++).
// Included at the end of <bits/stdc++.h>. Does nothing under libstdc++.
#pragma GCC system_header

#ifndef CPMAC_GNU_EXTENSIONS_H
#define CPMAC_GNU_EXTENSIONS_H

#include <cstddef>

#if defined(_LIBCPP_VERSION) && !defined(CPMAC_NO_GNU_EXTENSIONS)

#if __cplusplus >= 201402L
# define CPMAC_CONSTEXPR14 constexpr
#else
# define CPMAC_CONSTEXPR14 inline
#endif

namespace std {

// ---- std::__gcd ----------------------------------------------------------
// GCC: works on any integer type, same algorithm as below (so results,
// including signs, match a judge running GCC exactly).
// libc++: has an internal std::__gcd that rejects signed types. Non-template
// overloads are preferred over that template, so these take over cleanly.
#define CPMAC_DEFINE_GCD(T)                                   \
  CPMAC_CONSTEXPR14 T __gcd(T __m, T __n) {                   \
    while (__n != 0) { T __t = __m % __n; __m = __n; __n = __t; } \
    return __m;                                               \
  }
CPMAC_DEFINE_GCD(short)
CPMAC_DEFINE_GCD(unsigned short)
CPMAC_DEFINE_GCD(int)
CPMAC_DEFINE_GCD(unsigned int)
CPMAC_DEFINE_GCD(long)
CPMAC_DEFINE_GCD(unsigned long)
CPMAC_DEFINE_GCD(long long)
CPMAC_DEFINE_GCD(unsigned long long)
#ifdef __SIZEOF_INT128__
CPMAC_DEFINE_GCD(__int128)
CPMAC_DEFINE_GCD(unsigned __int128)
#endif
#undef CPMAC_DEFINE_GCD

// ---- std::__lg -----------------------------------------------------------
// floor(log2(n)) for n > 0, as used in sparse tables and binary lifting.
// Returns the argument's type, like GCC's.
#define CPMAC_DEFINE_LG(T, CLZ, BITS)                               \
  constexpr T __lg(T __n) { return static_cast<T>(BITS - 1 - CLZ(__n)); }
CPMAC_DEFINE_LG(int,                __builtin_clz,   int(sizeof(int) * 8))
CPMAC_DEFINE_LG(unsigned int,       __builtin_clz,   int(sizeof(int) * 8))
CPMAC_DEFINE_LG(long,               __builtin_clzl,  int(sizeof(long) * 8))
CPMAC_DEFINE_LG(unsigned long,      __builtin_clzl,  int(sizeof(long) * 8))
CPMAC_DEFINE_LG(long long,          __builtin_clzll, int(sizeof(long long) * 8))
CPMAC_DEFINE_LG(unsigned long long, __builtin_clzll, int(sizeof(long long) * 8))
#undef CPMAC_DEFINE_LG

} // namespace std

#undef CPMAC_CONSTEXPR14

// ---- namespace __gnu_cxx -------------------------------------------------
// Many CP templates contain `using namespace __gnu_cxx;`, which is a hard
// error when the namespace doesn't exist. Declaring it makes that line
// harmless. (__gnu_cxx::rope itself comes from <ext/rope>.)
namespace __gnu_cxx {}

#endif // _LIBCPP_VERSION
#endif // CPMAC_GNU_EXTENSIONS_H
