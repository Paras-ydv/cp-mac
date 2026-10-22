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
// Only safe before libc++ has been read at all: switching these on halfway
// leaves libc++ inconsistent (hard errors in <memory> at C++23). So with
// another standard header first, they stay off. cprun always puts this
// file first (-include), which avoids that.
#if !defined(CPMAC_NO_REMOVED_FEATURES) && !defined(_LIBCPP_VERSION)
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
// removed again in C++20; GCC still has them
# ifndef _LIBCPP_ENABLE_CXX20_REMOVED_NEGATORS
#  define _LIBCPP_ENABLE_CXX20_REMOVED_NEGATORS                // not1, not2
# endif
# ifndef _LIBCPP_ENABLE_CXX20_REMOVED_TYPE_TRAITS
#  define _LIBCPP_ENABLE_CXX20_REMOVED_TYPE_TRAITS             // result_of, is_literal_type
# endif
# ifndef _LIBCPP_ENABLE_CXX20_REMOVED_UNCAUGHT_EXCEPTION
#  define _LIBCPP_ENABLE_CXX20_REMOVED_UNCAUGHT_EXCEPTION      // std::uncaught_exception
# endif
# ifndef _LIBCPP_ENABLE_CXX20_REMOVED_BINDER_TYPEDEFS
#  define _LIBCPP_ENABLE_CXX20_REMOVED_BINDER_TYPEDEFS         // less<int>::first_argument_type etc.
# endif
# ifndef _LIBCPP_ENABLE_CXX20_REMOVED_RAW_STORAGE_ITERATOR
#  define _LIBCPP_ENABLE_CXX20_REMOVED_RAW_STORAGE_ITERATOR
# endif
# ifndef _LIBCPP_ENABLE_CXX20_REMOVED_TEMPORARY_BUFFER
#  define _LIBCPP_ENABLE_CXX20_REMOVED_TEMPORARY_BUFFER        // get_temporary_buffer
# endif
#endif

#endif // CPMAC_PRELUDE_H

// ---- -D_GLIBCXX_DEBUG / -D_GLIBCXX_ASSERTIONS on libc++ ------------------
// Many CP setups compile locally with -D_GLIBCXX_DEBUG to catch
// out-of-bounds vector access etc. libc++ ignores that macro, so map it to
// libc++'s own checks.
// Outside the include guard on purpose: cprun reads this file before the
// source (-include), so a `#define _GLIBCXX_DEBUG` at the top of the file is
// only seen when <bits/stdc++.h> includes it again. It's applied as long as
// libc++ hasn't been read yet.
#if (defined(_GLIBCXX_DEBUG) || defined(_GLIBCXX_ASSERTIONS)) && !defined(_LIBCPP_VERSION)
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
# elif (defined(__apple_build_version__) && __clang_major__ < 14) || \
       (!defined(__apple_build_version__) && __clang_major__ < 15)
   // libc++ 14 and older: assertions are switched on via _LIBCPP_DEBUG=0
#  ifndef _LIBCPP_DEBUG
#   define _LIBCPP_DEBUG 0
#  endif
# else
#  ifndef _LIBCPP_ENABLE_ASSERTIONS
#   define _LIBCPP_ENABLE_ASSERTIONS 1
#  endif
# endif
# undef CPMAC_LIBCXX_HAS_HARDENING
#endif
