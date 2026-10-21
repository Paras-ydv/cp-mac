// cp-mac: GCC/libstdc++ extensions that CP code relies on, recreated for
// libc++ (Apple Clang, Homebrew LLVM, clang -stdlib=libc++).
// Included at the end of <bits/stdc++.h>. Does nothing under libstdc++.
#pragma GCC system_header

#ifndef CPMAC_GNU_EXTENSIONS_H
#define CPMAC_GNU_EXTENSIONS_H

#include <cstddef>
#if __cplusplus >= 201103L
#include <type_traits>
#endif

#if defined(_LIBCPP_VERSION) && !defined(CPMAC_NO_GNU_EXTENSIONS)

#if __cplusplus >= 201402L
# define CPMAC_CONSTEXPR14 constexpr
#else
# define CPMAC_CONSTEXPR14 inline
#endif
#if __cplusplus >= 201103L
# define CPMAC_CONSTEXPR11 constexpr
#else
# define CPMAC_CONSTEXPR11 inline
#endif

namespace std {

// ---- std::__gcd ----------------------------------------------------------
// GCC: a template over any integer type, with the algorithm below (so
// results, including signs, match a judge running GCC exactly).
#if _LIBCPP_VERSION >= 220000 || __cplusplus < 201703L
// libc++ has no __gcd of its own here (22+, or before C++17), so define
// GCC's template as is: then __gcd<long long>(a, b), &__gcd<int> and a
// user's own ::__gcd overload all behave as they do with GCC.
template<typename _Tp>
CPMAC_CONSTEXPR14 _Tp __gcd(_Tp __m, _Tp __n) {
  while (__n != 0) { _Tp __t = __m % __n; __m = __n; __n = __t; }
  return __m;
}
#else
// libc++ 14-21 in C++17 and later have their own template std::__gcd (for std::gcd)
// that static_asserts on signed types. Specialise it for the signed types,
// so __gcd(a, b), __gcd<long long>(a, b) and &__gcd<int> work as with GCC;
// unsigned types keep libc++'s version. (A user's own non-template __gcd
// still wins over this template, as it does with GCC.)
#define CPMAC_DEFINE_GCD(T)                                       \
  template<> CPMAC_CONSTEXPR14 T __gcd<T>(T __m, T __n) {         \
    while (__n != 0) { T __t = __m % __n; __m = __n; __n = __t; } \
    return __m;                                                   \
  }
CPMAC_DEFINE_GCD(signed char)
CPMAC_DEFINE_GCD(short)
CPMAC_DEFINE_GCD(int)
CPMAC_DEFINE_GCD(long)
CPMAC_DEFINE_GCD(long long)
#ifdef __SIZEOF_INT128__
CPMAC_DEFINE_GCD(__int128)
#endif
#undef CPMAC_DEFINE_GCD
#endif

// ---- std::__lg -----------------------------------------------------------
// floor(log2(n)) for n > 0, as used in sparse tables and binary lifting.
// Like GCC's: a template returning the argument's type. (libc++ has none.)
CPMAC_CONSTEXPR11 int __cpmac_lg(unsigned long long __n) {
  return int(sizeof(unsigned long long) * 8) - 1 - __builtin_clzll(__n);
}
#ifdef __SIZEOF_INT128__
CPMAC_CONSTEXPR11 int __cpmac_lg(unsigned __int128 __n) {
  return (__n >> 64) != 0 ? 64 + __cpmac_lg(static_cast<unsigned long long>(__n >> 64))
                          : __cpmac_lg(static_cast<unsigned long long>(__n));
}
template<bool _Wide> struct __cpmac_lg_arg { typedef unsigned long long type; };
template<> struct __cpmac_lg_arg<true> { typedef unsigned __int128 type; };
#else
template<bool _Wide> struct __cpmac_lg_arg { typedef unsigned long long type; };
#endif

#if __cplusplus >= 201103L
// Through the same-width unsigned type, as GCC does (so __lg(-1) is 31 for int).
template<typename _Tp>
constexpr _Tp __lg(_Tp __n) {
  return static_cast<_Tp>(__cpmac_lg(
      static_cast<typename __cpmac_lg_arg<(sizeof(_Tp) > sizeof(long long))>::type>(
          static_cast<typename make_unsigned<_Tp>::type>(__n))));
}
#else
template<typename _Tp>
inline _Tp __lg(_Tp __n) {
  return static_cast<_Tp>(__cpmac_lg(static_cast<unsigned long long>(__n)));
}
#endif

} // namespace std

#undef CPMAC_CONSTEXPR14
#undef CPMAC_CONSTEXPR11

// ---- namespace __gnu_cxx -------------------------------------------------
// Many CP templates contain `using namespace __gnu_cxx;`, which is a hard
// error when the namespace doesn't exist. Declaring it makes that line
// harmless. (__gnu_cxx::rope itself comes from <ext/rope>.)
namespace __gnu_cxx {}

#endif // _LIBCPP_VERSION
#endif // CPMAC_GNU_EXTENSIONS_H
