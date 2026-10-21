// bits/stdc++.h — a drop-in replacement for GCC's <bits/stdc++.h>
// that works with Clang / Apple Clang (libc++) as well as GCC (libstdc++).
// Part of cp-mac: https://github.com/Paras-ydv/cp-mac (MIT License)
//
// Every header is wrapped in __has_include, so it compiles on any
// standard (-std=c++11 ... c++26) and any library version: headers your
// toolchain doesn't ship are simply skipped instead of breaking the build.

#ifndef CPMAC_BITS_STDCXX_H
#define CPMAC_BITS_STDCXX_H

// Apple Clang adds /usr/local/include with -I (not -isystem), so mark this
// as a system header to keep -Wall/-Wpedantic warnings out of user builds.
// (Not when it is itself the file being compiled, e.g. into a precompiled
// header: there the pragma only produces a warning.)
#if __INCLUDE_LEVEL__ > 0
#pragma GCC system_header
#endif

#ifndef __cplusplus
#error "<bits/stdc++.h> is a C++ header: compile with clang++ or g++, not clang or gcc."
// Real GCC has its own <bits/stdc++.h>; use it. (This copy is only seen
// first when cp-mac's include folder is passed with -I, e.g. Homebrew's.)
#elif defined(__GNUC__) && !defined(__clang__)
#include_next <bits/stdc++.h>
#else

#ifndef __has_include
  #error "This header needs a compiler with __has_include (GCC 5+, any Clang)."
#endif

// Must come before any standard header (re-enables removed C++98 helpers,
// maps -D_GLIBCXX_DEBUG to libc++'s checks).
#include <cp-mac/prelude.h>

// ---------- C headers (C++ wrappers) ----------
#include <cmath>   // before <cfloat>: Homebrew LLVM 22 + macOS 27 SDK lose INFINITY otherwise
#include <cassert>
#include <cctype>
#include <cerrno>
#include <cfloat>
#include <climits>
#include <clocale>
#include <cmath>
#include <csetjmp>
#include <csignal>
#include <cstdarg>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cwchar>
#include <cwctype>

#if __cplusplus >= 201103L
#include <cfenv>
#include <cinttypes>
#include <cstdint>
#if __has_include(<cuchar>)
#include <cuchar>
#endif
#endif

// ---------- C++98 ----------
#include <algorithm>
#include <bitset>
#include <complex>
#include <deque>
#include <exception>
#include <fstream>
#include <functional>
#include <iomanip>
#include <ios>
#include <iosfwd>
#include <iostream>
#include <istream>
#include <iterator>
#include <limits>
#include <list>
#include <locale>
#include <map>
#include <memory>
#include <new>
#include <numeric>
#include <ostream>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <streambuf>
#include <string>
#include <typeinfo>
#include <utility>
#include <valarray>
#include <vector>

// ---------- C++11 ----------
#if __cplusplus >= 201103L
#include <array>
#include <atomic>
#include <chrono>
#if __has_include(<codecvt>)
#include <codecvt>
#endif
#include <condition_variable>
#include <forward_list>
#include <future>
#include <initializer_list>
#include <mutex>
#include <random>
#include <ratio>
#include <regex>
#include <scoped_allocator>
#include <system_error>
#include <thread>
#include <tuple>
#include <type_traits>
#include <typeindex>
#include <unordered_map>
#include <unordered_set>
#endif

// ---------- C++14 ----------
#if __cplusplus >= 201402L
#include <shared_mutex>
#endif

// ---------- C++17 ----------
#if __cplusplus >= 201703L
#include <any>
#include <optional>
#include <string_view>
#include <variant>
#if __has_include(<charconv>)
#include <charconv>
#endif
#if __has_include(<filesystem>)
#include <filesystem>
#endif
#if __has_include(<memory_resource>)
#include <memory_resource>
#endif
#if __has_include(<execution>)
#include <execution>
#endif
#endif

// ---------- C++20 ----------
#if __cplusplus >= 202002L
#if __has_include(<barrier>)
#include <barrier>
#endif
#if __has_include(<bit>)
#include <bit>
#endif
#if __has_include(<compare>)
#include <compare>
#endif
#if __has_include(<concepts>)
#include <concepts>
#endif
#if __has_include(<coroutine>)
#include <coroutine>
#endif
#if __has_include(<format>)
#include <format>
#endif
#if __has_include(<latch>)
#include <latch>
#endif
#if __has_include(<numbers>)
#include <numbers>
#endif
#if __has_include(<ranges>)
#include <ranges>
#endif
#if __has_include(<semaphore>)
#include <semaphore>
#endif
#if __has_include(<source_location>)
#include <source_location>
#endif
#if __has_include(<span>)
#include <span>
#endif
#if __has_include(<stop_token>)
#include <stop_token>
#endif
#if __has_include(<syncstream>)
#include <syncstream>
#endif
#if __has_include(<version>)
#include <version>
#endif
#endif

// ---------- C++23 ----------
#if __cplusplus > 202002L
#if __has_include(<expected>)
#include <expected>
#endif
#if __has_include(<flat_map>)
#include <flat_map>
#endif
#if __has_include(<flat_set>)
#include <flat_set>
#endif
#if __has_include(<generator>)
#include <generator>
#endif
#if __has_include(<mdspan>)
#include <mdspan>
#endif
#if __has_include(<print>)
#include <print>
#endif
#if __has_include(<spanstream>)
#include <spanstream>
#endif
#if __has_include(<stacktrace>)
#include <stacktrace>
#endif
#if __has_include(<stdfloat>)
#include <stdfloat>
#endif
#endif


// ---------- C++26 ----------
#if __cplusplus > 202302L
#if __has_include(<debugging>)
#include <debugging>
#endif
#if __has_include(<inplace_vector>)
#include <inplace_vector>
#endif
#if __has_include(<linalg>)
#include <linalg>
#endif
#if __has_include(<text_encoding>)
#include <text_encoding>
#endif
#endif

// GCC extensions CP code expects (std::__gcd, std::__lg, ...) on libc++.
#include <cp-mac/gnu_extensions.h>

#endif // C / real GCC / everything else
#endif // CPMAC_BITS_STDCXX_H
