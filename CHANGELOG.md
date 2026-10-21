# Changelog

## 1.0.0

First release.

**Headers**
- `#include <bits/stdc++.h>` and `<bits/extc++.h>` for Apple Clang, for every `-std` from C++98 to C++26. Real GCC keeps using its own.
- pb_ds (`ordered_set` / `tree`, `gp_hash_table`, `cc_hash_table`, `trie`, `__gnu_pbds::priority_queue`) and rope / crope: GCC's own implementation, so results match the judge.
- `std::__gcd` and `std::__lg` as GCC-style templates, with GCC's results.
- `random_shuffle`, `bind1st`/`bind2nd`, `unary_function`, `ptr_fun`, `auto_ptr`, `not1`, `result_of` keep working in C++17 and later.
- `-D_GLIBCXX_DEBUG` turns on libc++'s bounds checks.

**`cprun`** (also `cp-mac run`)
- Compiles with CP flags and a 256 MB stack, so deep recursion doesn't crash.
- Precompiles `<bits/stdc++.h>`, so recompiling after an edit is several times faster.
- Prints CPU time and peak memory after each run, and explains crashes.
- `-d` for a debug build with AddressSanitizer and UBSan.

**Install and maintenance**
- One-line installer (`curl | sh`) and Homebrew (`brew install paras-ydv/cp/cp-mac`, then `cp-mac link` once on Apple Silicon).
- `cp-mac doctor` checks the setup and says how to fix what's wrong.
- Notices when a new release is out; `cp-mac update`, and opt-in daily updates with `cp-mac autoupdate on`.
- Clean `cp-mac uninstall` for either install method.
