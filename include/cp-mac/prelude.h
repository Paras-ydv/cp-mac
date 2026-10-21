// cp-mac prelude: settings that must be in place BEFORE the first standard
// header is read. Included at the very top of <bits/stdc++.h>, so it takes
// effect whenever that is your first #include (the usual CP setup).
//
// Everything here is a no-op with GCC/libstdc++, which never removed these.
#pragma GCC system_header

#ifndef CPMAC_PRELUDE_H
#define CPMAC_PRELUDE_H

// ---- Keep old-but-common CP code compiling on C++17 and later ------------
// libstdc++ only deprecates these; libc++ deletes them, turning lots of
// older templates and editorials into compile errors on a Mac.
#ifndef CPMAC_NO_REMOVED_FEATURES
# ifndef _LIBCPP_ENABLE_CXX17_REMOVED_RANDOM_SHUFFLE
#  define _LIBCPP_ENABLE_CXX17_REMOVED_RANDOM_SHUFFLE          // std::random_shuffle
# endif
# ifndef _LIBCPP_ENABLE_CXX17_REMOVED_UNARY_BINARY_FUNCTION
#  define _LIBCPP_ENABLE_CXX17_REMOVED_UNARY_BINARY_FUNCTION   // std::unary_function / binary_function
# endif
# ifndef _LIBCPP_ENABLE_CXX17_REMOVED_BINDERS
#  define _LIBCPP_ENABLE_CXX17_REMOVED_BINDERS                 // bind1st, bind2nd, ptr_fun, mem_fun
# endif
# ifndef _LIBCPP_ENABLE_CXX17_REMOVED_AUTO_PTR
#  define _LIBCPP_ENABLE_CXX17_REMOVED_AUTO_PTR                // std::auto_ptr
# endif
#endif

// ---- -D_GLIBCXX_DEBUG / -D_GLIBCXX_ASSERTIONS on libc++ ------------------
// Many CP setups compile locally with -D_GLIBCXX_DEBUG to catch
// out-of-bounds vector access etc. libc++ ignores that macro, so map it to
// libc++'s own checks.
#if defined(_GLIBCXX_DEBUG) || defined(_GLIBCXX_ASSERTIONS)
# if defined(__apple_build_version__)
#  define CPMAC_LIBCXX_HAS_HARDENING (__clang_major__ >= 16)
# else
#  define CPMAC_LIBCXX_HAS_HARDENING (__clang_major__ >= 18)
# endif
# if CPMAC_LIBCXX_HAS_HARDENING
#  ifndef _LIBCPP_HARDENING_MODE
#   ifdef _GLIBCXX_DEBUG
#    define _LIBCPP_HARDENING_MODE _LIBCPP_HARDENING_MODE_DEBUG
#   else
#    define _LIBCPP_HARDENING_MODE _LIBCPP_HARDENING_MODE_EXTENSIVE
#   endif
#  endif
# else
#  ifndef _LIBCPP_ENABLE_ASSERTIONS
#   define _LIBCPP_ENABLE_ASSERTIONS 1
#  endif
# endif
# undef CPMAC_LIBCXX_HAS_HARDENING
#endif

#endif // CPMAC_PRELUDE_H
