# cp-mac

**Competitive programming on a Mac, without the GCC-only errors.**

On macOS, `g++` is really Apple Clang, so code that works on every judge fails locally:

```
fatal error: 'bits/stdc++.h' file not found
fatal error: 'ext/pb_ds/assoc_container.hpp' file not found
error: no member named '__lg' in namespace 'std'
error: no member named 'random_shuffle' in namespace 'std'
Segmentation fault: 11          ← deep DFS, because macOS gives you an 8 MB stack
```

cp-mac fixes all of these with one install. You keep using Apple Clang, with no GCC download and no editor reconfiguration.

[![CI](https://github.com/Paras-ydv/cp-mac/actions/workflows/ci.yml/badge.svg)](https://github.com/Paras-ydv/cp-mac/actions/workflows/ci.yml)

## Install

**One command** (recommended; works on Intel and Apple Silicon):

```sh
curl -fsSL https://raw.githubusercontent.com/Paras-ydv/cp-mac/main/install.sh | sh
```

**Homebrew:**

```sh
brew install paras-ydv/tap/cp-mac
cp-mac link        # Apple Silicon only: lets Apple Clang find Homebrew's headers
```

**From source:**

```sh
git clone https://github.com/Paras-ydv/cp-mac && cd cp-mac && make install
```

Then check that everything works:

```sh
cp-mac doctor
```

You need Apple's compiler (`xcode-select --install`), which you most likely already have.

## What you get

| Works after installing | Notes |
|---|---|
| `#include <bits/stdc++.h>` | Adapts to your Clang version and `-std=` (C++11 to C++26), so it doesn't break when Xcode updates |
| `#include <bits/extc++.h>` | Everything above + pb_ds + rope |
| **pb_ds**: `ordered_set` (`tree` + `find_by_order` / `order_of_key`), `gp_hash_table`, `cc_hash_table`, `trie`, `__gnu_pbds::priority_queue` (with `join` / `modify`) | GCC's real implementation, so behaviour matches the judge |
| **rope**: `__gnu_cxx::rope`, `crope` | GCC's real implementation; also usable as an `unordered_set` key |
| `std::__gcd`, `std::__lg` | `__gcd` gives the same results as GCC, including signs; `__lg` is `constexpr` |
| `random_shuffle`, `unary_function`, `binary_function`, `bind1st`, `bind2nd`, `ptr_fun`, `auto_ptr` in C++17/20 | GCC only deprecates these; Apple's library deletes them |
| `using namespace __gnu_cxx;` | No longer an error |
| `-D_GLIBCXX_DEBUG` | Turns on libc++'s bounds checks, like GCC's debug mode |
| `cprun` | Compile and run with CP flags and a **256 MB stack** |

## Usage

Write code exactly as you would for Codeforces / AtCoder / CodeChef:

```cpp
#include <bits/stdc++.h>
#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/tree_policy.hpp>
using namespace std;
using namespace __gnu_pbds;

template <class T>
using ordered_set = tree<T, null_type, less<T>, rb_tree_tag, tree_order_statistics_node_update>;

int main() {
    ordered_set<int> s;
    for (int x : {5, 1, 9, 3}) s.insert(x);
    cout << *s.find_by_order(1) << " " << s.order_of_key(6) << "\n";  // 3 3
}
```

Compile it however you like. `clang++ a.cpp`, `g++ a.cpp`, VS Code and CLion all work with no extra flags. Or use `cprun`:

```sh
cprun a.cpp < in.txt          # -std=c++17 -O2 -Wall -DLOCAL, big stack, recompiles only when changed
cprun -s c++20 a.cpp          # another standard
cprun -d a.cpp < in.txt       # debug: -D_GLIBCXX_DEBUG + AddressSanitizer + UBSan
```

### Why `cprun`? The stack

macOS gives programs an 8 MB stack. Codeforces gives 256 MB. A recursive DFS on a 10⁵–10⁶ node path tree works on the judge and crashes on your Mac. `cprun` links with `-Wl,-stack_size,0x10000000` (256 MB). If you use your own build command, add that flag.

## Editors

- **VS Code** (C/C++ extension): set the compiler path to `/usr/bin/clang++`. IntelliSense picks up `/usr/local/include` automatically. If you use Code Runner, set its C++ command to `cprun $fileName`.
- **CLion / Xcode / Sublime / Vim**: nothing to configure. Headers are in a folder Apple Clang searches by default.
- **Homebrew on Apple Silicon**: run `cp-mac link` once, or the editor won't see the headers.

## Commands

```
cp-mac run FILE.cpp   compile & run (alias: cprun)
cp-mac doctor         check everything works and explain what doesn't
cp-mac link | unlink  (Homebrew on Apple Silicon) expose headers to Apple Clang
cp-mac update         update to the latest version
cp-mac uninstall      remove everything cp-mac installed
```

## Differences from GCC you should know about

cp-mac can't change the hardware:

- **`long double`** is 64-bit on Apple Silicon (same as `double`), but 80-bit on x86 judges. Precision-sensitive answers can differ.
- **`#pragma GCC target("avx2")` / `optimize`** do nothing with Clang (you'll see a warning); AVX doesn't exist on Apple Silicon.
- pb_ds's own internal debug checks stay off under `-D_GLIBCXX_DEBUG`; the standard containers are still checked.

## How it works

- `include/bits/stdc++.h` includes every standard header your compiler actually has, by checking with `__has_include`. Before that, it enables libc++'s compatibility switches for removed C++98 helpers.
- `include/ext/` is **GCC's own pb_ds and rope source**, run through [`scripts/vendor-gcc.sh`](scripts/vendor-gcc.sh). The script mechanically swaps libstdc++-internal names for small standard-library-based stand-ins in `include/cp-mac/`. No data structure code is rewritten, so behaviour matches GCC.
- Everything installs into `/usr/local/include`, which Apple Clang searches out of the box, so no flags or environment variables are needed.
- If you also use real GCC (`g++-14` from Homebrew), it keeps using its own headers; cp-mac doesn't interfere.

CI runs the full test suite on real macOS 14, 15 and 26 (Apple Silicon) and macOS 15 Intel every week, so breakage from new Xcode versions shows up quickly.

## Uninstall

```sh
cp-mac uninstall          # script / make install
brew uninstall cp-mac     # Homebrew (run `cp-mac unlink` first if you linked)
```

## Prior art

Many people have solved parts of this before, mostly by copying GCC's header by hand. These projects showed the demand and the approaches:
[khaveesh/macOS-stdc.h](https://github.com/khaveesh/macOS-stdc.h),
[prsweet/std-with-pbds](https://github.com/prsweet/std-with-pbds),
[tekfyl/bits-stdc-.h-for-mac](https://github.com/tekfyl/bits-stdc-.h-for-mac),
[makeavish/bits-stdc-.h-for-mac](https://github.com/makeavish/bits-stdc-.h-for-mac).
cp-mac aims to be the complete, tested, one-command version: rope, the C++17 removals, `__lg`, the stack size, package-manager installs, and CI on real Macs.

## Contributing

```sh
make test                          # run the tests against this checkout
CXX="clang++ -stdlib=libc++" make test
scripts/vendor-gcc.sh /path/to/gcc # update pb_ds / rope from a newer GCC
```

Found GCC-only code that still doesn't compile on your Mac? Open an issue with the snippet.

## License

cp-mac's own code is [MIT](LICENSE). The pb_ds and rope headers in `include/ext/` are GCC's, under GPLv3+ with the GCC Runtime Library Exception, the same terms GCC ships them under. See [third_party/gcc](third_party/gcc/).
