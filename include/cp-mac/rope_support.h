// cp-mac: the libstdc++ internals and SGI extensions that GCC's <ext/rope>
// needs, reimplemented on top of the standard library so the rope sources
// compile with Clang + libc++. scripts/vendor-gcc.sh points the rope
// sources at the __cpmac_rope:: names below.
//
// Part of cp-mac (MIT License).
#pragma GCC system_header

#ifndef CPMAC_ROPE_SUPPORT_H
#define CPMAC_ROPE_SUPPORT_H

#include <cp-mac/libstdcxx_compat.h>

#include <algorithm>
#include <cstddef>
#include <cstring>
#include <functional>
#include <iosfwd>
#include <iterator>
#include <memory>
#include <new>
#include <ostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <pthread.h>

namespace __cpmac_rope
{
  // std::binary_function is gone from libc++ in C++17; rope derives from it.
  template<typename _Arg1, typename _Arg2, typename _Result>
    struct binary_function
    {
      typedef _Arg1 first_argument_type;
      typedef _Arg2 second_argument_type;
      typedef _Result result_type;
    };

  // __gnu_cxx::__alloc_traits
  template<typename _Alloc>
    struct __alloc_traits : std::allocator_traits<_Alloc>
    {
      template<typename _Tp>
        struct rebind
        { typedef typename std::allocator_traits<_Alloc>::template rebind_alloc<_Tp> other; };
    };

  // std::_Destroy
  template<typename _ForwardIterator>
    inline void
    _Destroy(_ForwardIterator __first, _ForwardIterator __last)
    {
      typedef typename std::iterator_traits<_ForwardIterator>::value_type _Value;
      for (; __first != __last; ++__first)
        std::addressof(*__first)->~_Value();
    }

  template<typename _ForwardIterator, typename _Alloc>
    inline void
    _Destroy(_ForwardIterator __first, _ForwardIterator __last, _Alloc& __alloc)
    {
      for (; __first != __last; ++__first)
        std::allocator_traits<_Alloc>::destroy(__alloc, std::addressof(*__first));
    }

  // std::__uninitialized_copy_n_a
  template<typename _InputIterator, typename _Size, typename _ForwardIterator,
           typename _Alloc>
    inline _ForwardIterator
    __uninitialized_copy_n_a(_InputIterator __first, _Size __n,
                             _ForwardIterator __result, _Alloc& __alloc)
    {
      _ForwardIterator __cur = __result;
      __CPMAC_TRY
        {
          for (; __n > 0; --__n, (void) ++__first, ++__cur)
            std::allocator_traits<_Alloc>::construct(__alloc, std::addressof(*__cur), *__first);
          return __cur;
        }
      __CPMAC_CATCH(...)
        {
          _Destroy(__result, __cur, __alloc);
          __CPMAC_RETHROW;
        }
      return __cur;
    }

  // std::__uninitialized_fill_n_a
  template<typename _ForwardIterator, typename _Size, typename _Tp,
           typename _Alloc>
    inline _ForwardIterator
    __uninitialized_fill_n_a(_ForwardIterator __first, _Size __n,
                             const _Tp& __x, _Alloc& __alloc)
    {
      _ForwardIterator __cur = __first;
      __CPMAC_TRY
        {
          for (; __n > 0; --__n, (void) ++__cur)
            std::allocator_traits<_Alloc>::construct(__alloc, std::addressof(*__cur), __x);
          return __cur;
        }
      __CPMAC_CATCH(...)
        {
          _Destroy(__first, __cur, __alloc);
          __CPMAC_RETHROW;
        }
      return __cur;
    }

  // __gnu_cxx::uninitialized_copy_n (SGI extension: returns both ends)
  template<typename _InputIterator, typename _Size, typename _ForwardIterator>
    inline std::pair<_InputIterator, _ForwardIterator>
    uninitialized_copy_n(_InputIterator __first, _Size __n,
                         _ForwardIterator __result)
    {
      typedef typename std::iterator_traits<_ForwardIterator>::value_type _Value;
      _ForwardIterator __cur = __result;
      __CPMAC_TRY
        {
          for (; __n > 0; --__n, (void) ++__first, ++__cur)
            ::new (static_cast<void*>(std::addressof(*__cur))) _Value(*__first);
          return std::pair<_InputIterator, _ForwardIterator>(__first, __cur);
        }
      __CPMAC_CATCH(...)
        {
          _Destroy(__result, __cur);
          __CPMAC_RETHROW;
        }
      return std::pair<_InputIterator, _ForwardIterator>(__first, __cur);
    }

  // __gnu_cxx::copy_n (SGI extension: returns both ends)
  template<typename _InputIterator, typename _Size, typename _OutputIterator>
    inline std::pair<_InputIterator, _OutputIterator>
    copy_n(_InputIterator __first, _Size __count, _OutputIterator __result)
    {
      for (; __count > 0; --__count, (void) ++__first, ++__result)
        *__result = *__first;
      return std::pair<_InputIterator, _OutputIterator>(__first, __result);
    }

  // __gnu_cxx::lexicographical_compare_3way: <0, 0 or >0, like strcmp
  template<typename _InputIterator1, typename _InputIterator2>
    inline int
    lexicographical_compare_3way(_InputIterator1 __first1, _InputIterator1 __last1,
                                 _InputIterator2 __first2, _InputIterator2 __last2)
    {
      while (__first1 != __last1 && __first2 != __last2)
        {
          if (*__first1 < *__first2)
            return -1;
          if (*__first2 < *__first1)
            return 1;
          ++__first1;
          ++__first2;
        }
      if (__first2 == __last2)
        return !(__first1 == __last1);
      return -1;
    }

  // std::__throw_length_error
  [[noreturn]] inline void
  __throw_length_error(const char* __s)
  {
#if defined(__cpp_exceptions) || defined(__EXCEPTIONS)
    throw std::length_error(__s);
#else
    (void) __s;
    std::abort();
#endif
  }
} // namespace __cpmac_rope

#endif // CPMAC_ROPE_SUPPORT_H
