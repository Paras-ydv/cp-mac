// cp-mac: stand-ins for the libstdc++ internals that GCC's extension headers
// (pb_ds, rope) use, so those mostly-unmodified GCC sources compile with
// Clang + libc++. scripts/vendor-gcc.sh renames the internal names in the
// GCC sources to the __CPMAC_* / __cpmac names defined here, so nothing in
// here can clash with a real libstdc++.
//
// Part of cp-mac (MIT License).
#pragma GCC system_header

#ifndef CPMAC_LIBSTDCXX_COMPAT_H
#define CPMAC_LIBSTDCXX_COMPAT_H

// pb_ds / rope included before <bits/stdc++.h>: still apply the prelude.
#include <cp-mac/prelude.h>

#include <cassert>
#include <climits>
#include <cstddef>
#include <cstdlib>
#include <exception>
#include <limits>
#include <type_traits>

// ---- libstdc++ configuration macros -------------------------------------
#define __CPMAC_VISIBILITY(V)
#define __CPMAC_BEGIN_NAMESPACE_VERSION
#define __CPMAC_END_NAMESPACE_VERSION
#define __CPMAC_N(msgid) (msgid)

#if __cplusplus >= 201703L
# define __CPMAC_NODISCARD [[__nodiscard__]]
#else
# define __CPMAC_NODISCARD
#endif

// ---- debug checks --------------------------------------------------------
// pb_ds' own debug mode needs libstdc++'s throw_allocator machinery, so it
// is always compiled out here (even with -D_GLIBCXX_DEBUG; the standard
// containers still get libc++'s checks via cp-mac/prelude.h).
#define __CPMAC_DEBUG_ASSERT(_Condition)
#define __CPMAC_DEBUG_ONLY(_Statement)
#define __CPMAC_DEBUG_VERIFY_AT(_Condition, _ErrMsg, _File, _Line)

// ---- exceptions (honours -fno-exceptions like libstdc++ does) -----------
#if defined(__cpp_exceptions) || defined(__EXCEPTIONS)
# define __CPMAC_TRY try
# define __CPMAC_CATCH(X) catch (X)
# define __CPMAC_RETHROW throw
# define __CPMAC_THROW_OR_ABORT(_EXC) (throw (_EXC))
#else
# define __CPMAC_TRY if (true)
# define __CPMAC_CATCH(X) if (false)
# define __CPMAC_RETHROW
# define __CPMAC_THROW_OR_ABORT(_EXC) (std::abort())
#endif

// ---- __gnu_cxx type helpers that pb_ds borrows --------------------------
namespace __cpmac
{
  template<bool _Cond, typename _Iftrue, typename _Iffalse>
    struct __conditional_type
    { typedef _Iftrue __type; };

  template<typename _Iftrue, typename _Iffalse>
    struct __conditional_type<false, _Iftrue, _Iffalse>
    { typedef _Iffalse __type; };

  template<typename _Tp>
    struct __numeric_traits
    {
      typedef std::numeric_limits<_Tp> __lim;
      static const _Tp __min = __lim::min();
      static const _Tp __max = __lim::max();
      static const bool __is_signed = __lim::is_signed;
      static const int __digits = __lim::digits;
    };

  template<typename _Tp> const _Tp __numeric_traits<_Tp>::__min;
  template<typename _Tp> const _Tp __numeric_traits<_Tp>::__max;
  template<typename _Tp> const bool __numeric_traits<_Tp>::__is_signed;
  template<typename _Tp> const int __numeric_traits<_Tp>::__digits;
} // namespace __cpmac

#endif // CPMAC_LIBSTDCXX_COMPAT_H
