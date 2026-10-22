#!/bin/sh
# Runs the cp-mac test suite.
#
#   tests/run.sh               test the headers in this repo (adds -Iinclude)
#   tests/run.sh --installed   test whatever the compiler finds on its own,
#                              i.e. check that an install actually works
#
#   CXX=clang++ tests/run.sh   pick the compiler (default: c++)
set -u

cd "$(dirname "$0")/.." || exit 1
CXX="${CXX:-c++}"
INC="-Iinclude"
[ "${1:-}" = "--installed" ] && INC=""

OUT="$(mktemp -d)"
trap 'rm -rf "$OUT"' EXIT
FLAGS="-O2 -Wall -Wextra -Wshadow -Werror -Wno-deprecated-declarations"
# GCC 16 reports a false -Warray-bounds inside its own pb_ds trie code at -O2.
$CXX --version 2>/dev/null | grep -qi clang || FLAGS="$FLAGS -Wno-array-bounds"
pass=0; fail=0

supports() {
  echo 'int main(){}' > "$OUT/empty.cpp"
  $CXX -std="$1" -fsyntax-only "$OUT/empty.cpp" >/dev/null 2>&1
}

run() {  # run <name> <std> <file> [extra flags...]
  name="$1"; std="$2"; src="$3"; shift 3
  if $CXX -std="$std" $FLAGS $INC "$@" "$src" -o "$OUT/a.out" >"$OUT/log" 2>&1 \
     && ( "$OUT/a.out" >"$OUT/run" 2>&1; exit $? ) 2>/dev/null && grep -q ': OK' "$OUT/run"; then
    printf '  ok    %-12s %s\n' "$name" "$std"; pass=$((pass + 1))
  else
    printf '  FAIL  %-12s %s\n' "$name" "$std"; fail=$((fail + 1))
    sed 's/^/        /' "$OUT/log" "$OUT/run" 2>/dev/null | head -8
  fi
}

echo "Compiler: $($CXX --version 2>&1 | head -1)"
echo "Headers:  ${INC:-installed (no -I flag)}"

for std in c++11 c++14 c++17 c++20 c++23 c++2b c++26 c++2c; do
  supports "$std" || continue
  [ "$std" = c++2b ] && supports c++23 && continue   # same thing, skip dup
  [ "$std" = c++2c ] && supports c++26 && continue
  run pbds "$std" tests/pbds.cpp
  [ "$std" = c++11 ] && continue                     # rest use C++14 syntax
  run stdc "$std" tests/stdc.cpp
  run extc "$std" tests/extc.cpp
  run rope "$std" tests/rope.cpp
  run gcc-compat "$std" tests/gcc_compat.cpp
  run order "$std" tests/include_order.cpp
done

# precompiling <bits/stdc++.h> itself (what cprun does with clang) must be warning-free
if ! $CXX --version 2>/dev/null | grep -qi clang; then :
elif $CXX -std=c++17 $FLAGS $INC -x c++-header "${INC:+include/}bits/stdc++.h" -o "$OUT/s.pch" >"$OUT/log" 2>&1 || \
   { [ -z "$INC" ] && echo '#include <bits/stdc++.h>' | $CXX -std=c++17 $FLAGS -x c++-header - -o "$OUT/s.pch" >"$OUT/log" 2>&1; }; then
  printf '  ok    %-12s %s\n' pch "precompiled header builds with -Werror"; pass=$((pass + 1))
else
  printf '  FAIL  %-12s %s\n' pch "precompiled header"; fail=$((fail + 1)); sed 's/^/        /' "$OUT/log" | head -8
fi

# with exceptions disabled
run pbds-noexc c++17 tests/pbds.cpp -fno-exceptions
run rope-noexc c++17 tests/rope.cpp -fno-exceptions

# -D_GLIBCXX_DEBUG must catch an out-of-bounds access (non-zero exit)
if $CXX -std=c++17 $FLAGS $INC -D_GLIBCXX_DEBUG tests/debug_mode.cpp -o "$OUT/dbg" >"$OUT/log" 2>&1; then
  if ( "$OUT/dbg" >"$OUT/run" 2>&1; exit $? ) 2>/dev/null; then   # (hides the shell's "Abort trap" line)
    printf '  FAIL  %-12s %s\n' debug-mode "out-of-bounds access was not caught"; fail=$((fail + 1))
  else
    printf '  ok    %-12s %s\n' debug-mode "out-of-bounds access caught"; pass=$((pass + 1))
  fi
else
  printf '  FAIL  %-12s %s\n' debug-mode "did not compile"; fail=$((fail + 1))
  sed 's/^/        /' "$OUT/log" | head -8
fi

echo "$pass passed, $fail failed"
[ "$fail" -eq 0 ]
