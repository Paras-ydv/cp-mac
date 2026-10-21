#!/usr/bin/env bash
# Re-imports GCC's extension headers that CP code uses -- policy-based data
# structures (ext/pb_ds) and rope (ext/rope) -- and patches them so they
# compile with Clang + libc++ (what macOS uses).
#
# Usage: scripts/vendor-gcc.sh /path/to/gcc-source-checkout
#
# The patches are mechanical renames, so moving to a newer GCC is just a
# matter of re-running this script and the tests:
#   * libstdc++-internal #includes  -> cp-mac compat headers
#   * _GLIBCXX_* macros             -> __CPMAC_*   (cp-mac/libstdcxx_compat.h)
#   * __try / __catch / ...         -> __CPMAC_TRY / __CPMAC_CATCH / ...
#   * std::tr1::                    -> std::
#   * libstdc++ internals used by rope -> __cpmac_rope:: (cp-mac/rope_support.h)
#   * every file is marked as a system header so -Wall stays quiet
set -euo pipefail

GCC_SRC="${1:?usage: $0 /path/to/gcc-source}"
SRC_INC="$GCC_SRC/libstdc++-v3/include"
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
EXT="$ROOT/include/ext"
PRIV="$EXT/pb_ds/detail/cpmac"

for f in ext/pb_ds ext/rope ext/ropeimpl.h ext/typelist.h; do
  [ -e "$SRC_INC/$f" ] || { echo "missing $SRC_INC/$f" >&2; exit 1; }
done

rm -rf "$EXT"
mkdir -p "$EXT"
cp -R "$SRC_INC/ext/pb_ds" "$EXT/pb_ds"
cp "$SRC_INC/ext/rope" "$SRC_INC/ext/ropeimpl.h" "$EXT/"
mkdir -p "$PRIV"
cp "$SRC_INC/ext/typelist.h" "$PRIV/typelist.hpp"   # only used inside pb_ds

FILES=$(find "$EXT" -type f)

# ---- renames shared by everything ------------------------------------------
# shellcheck disable=SC2086
perl -0pi -e '
  s{#\s*include\s*<bits/c\+\+config\.h>}{#include <cp-mac/libstdcxx_compat.h>}g;
  s{#\s*include\s*<debug/debug\.h>}{#include <cp-mac/libstdcxx_compat.h>}g;
  s{#\s*include\s*<ext/type_traits\.h>}{#include <cp-mac/libstdcxx_compat.h>}g;
  s{#\s*include\s*<ext/numeric_traits\.h>}{#include <cp-mac/libstdcxx_compat.h>}g;
  s{#\s*include\s*<ext/throw_allocator\.h>}{#include <cp-mac/libstdcxx_compat.h>}g;
  s{#\s*include\s*<ext/typelist\.h>}{#include <ext/pb_ds/detail/cpmac/typelist.hpp>}g;
  s{#\s*include\s*<tr1/type_traits>}{#include <type_traits>}g;
  s{std::tr1::}{std::}g;
  s{std::_GLIBCXX_STD_C::}{std::}g;
  s{__gnu_cxx::typelist}{__cpmac::typelist}g;
  s{__gnu_cxx::__conditional_type}{__cpmac::__conditional_type}g;
  s{__gnu_cxx::__numeric_traits}{__cpmac::__numeric_traits}g;
  s{\b_GLIBCXX_}{__CPMAC_}g;
  s{\b__try\b}{__CPMAC_TRY}g;
  s{\b__catch\b}{__CPMAC_CATCH}g;
  s{\b__throw_exception_again\b}{__CPMAC_RETHROW}g;
  s{\b__N\(}{__CPMAC_N(}g;
' $FILES

# pb_ds: tr1/functional -> <functional>
find "$EXT/pb_ds" -type f -print0 | xargs -0 perl -0pi -e '
  s{#\s*include\s*<tr1/functional>}{#include <functional>}g;
'

# trie_order_statistics_node_update: allocator rebind<> is deprecated in
# C++17 and gone in C++20; use pb_ds' own rebind_traits like the rest of pb_ds.
perl -0pi -e '
  s{typedef typename _Alloc::template rebind<metadata_type> __rebind_m;\n\s*typedef typename __rebind_m::other\s+__rebind_ma;}{typedef detail::rebind_traits<_Alloc, metadata_type> __rebind_ma;}g;
' "$EXT/pb_ds/trie_policy.hpp"

# typelist.hpp: move it out of namespace __gnu_cxx, and give it its own
# include guard (GCC's <ext/typelist.h> uses _TYPELIST_H; both can meet when
# real GCC is pointed at cp-mac's headers with -I).
perl -0pi -e '
  s{\b_TYPELIST_H\b}{CPMAC_PBDS_TYPELIST_HPP}g;
  s{namespace __gnu_cxx\b}{namespace __cpmac}g;
  s{__gnu_cxx::}{__cpmac::}g;
' "$PRIV/typelist.hpp"

# ---- rope ------------------------------------------------------------------
perl -0pi -e '
  # libstdc++ internal headers and SGI extension headers -> rope_support.h
  s{#\s*include\s*<bits/[^>]+>[^\n]*\n}{}g;
  s{#\s*include\s*<(?:ext/alloc_traits\.h|ext/memory|ext/algorithm|ext/numeric|tr1/functional)>[^\n]*\n}{}g;
  s{(#define _ROPE 1\n)}{$1\n#include <cp-mac/rope_support.h>\n};
  # gthreads -> pthreads
  s{\b__gthread_mutex_}{pthread_mutex_}g;
  s{\b__GTHREAD_MUTEX_INIT\b}{PTHREAD_MUTEX_INITIALIZER}g;
  # internals -> our implementations
  s{std::_Destroy\b}{__cpmac_rope::_Destroy}g;
  s{using std::__uninitialized_fill_n_a;}{using __cpmac_rope::__uninitialized_fill_n_a;}g;
  s{(?<![\w:])__uninitialized_copy_n_a\(}{__cpmac_rope::__uninitialized_copy_n_a(}g;
  s{(?<![\w:])uninitialized_copy_n\(}{__cpmac_rope::uninitialized_copy_n(}g;
  s{(?<![\w:])copy_n\(}{__cpmac_rope::copy_n(}g;
  s{(?<![\w:])lexicographical_compare_3way\(}{__cpmac_rope::lexicographical_compare_3way(}g;
  s{(?<![\w:])power\(}{__cpmac_rope::__power(}g;          # SGI <ext/numeric>, used by rope(n, c)
  s{(?<![\w:])__alloc_traits<}{__cpmac_rope::__alloc_traits<}g;
  s{std::__throw_length_error\b}{__cpmac_rope::__throw_length_error}g;
  s{std::binary_function\b}{__cpmac_rope::binary_function}g;
  # std::tr1::hash<crope> -> std::hash<crope> (usable in unordered_map)
  s{\nnamespace tr1\n\{\n}{\n}g;
  s{\} // namespace tr1\n}{}g;
' "$EXT/rope" "$EXT/ropeimpl.h"

# ---- mark everything as a system header ------------------------------------
# (Apple Clang puts /usr/local/include on -I, so warnings would otherwise
# leak into user builds.)
# Also stamp each file as modified (GPLv3 section 5a).
GCC_REV="$(git -C "$GCC_SRC" log -1 --format='%H' 2>/dev/null || echo unknown)"
TODAY="$(date +%Y-%m-%d)"
export GCC_REV TODAY
find "$EXT" -type f -print0 |
  xargs -0 perl -pi -e 'if ($. == 1) { print "#pragma GCC system_header\n// Modified by cp-mac on $ENV{TODAY} (scripts/vendor-gcc.sh, from GCC $ENV{GCC_REV}):\n// libstdc++-internal names replaced so this compiles with Clang + libc++.\n" } close ARGV if eof'

printf '%s\n' "$GCC_REV" > "$ROOT/third_party/gcc/GCC_REVISION"
echo "Vendored from GCC ${GCC_REV:0:12}"
