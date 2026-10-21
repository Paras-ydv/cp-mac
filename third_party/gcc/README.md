# Vendored GCC headers

`include/ext/pb_ds/` (policy-based data structures) and `include/ext/rope`,
`include/ext/ropeimpl.h` are taken from GCC's libstdc++, at the commit in
[`GCC_REVISION`](GCC_REVISION).

They are licensed under the **GNU General Public License v3 or later with the
GCC Runtime Library Exception 3.1** ([`COPYING3`](COPYING3),
[`COPYING.RUNTIME`](COPYING.RUNTIME)), the same terms GCC ships them under.
Their original copyright and permission notices are kept in every file.

## Modifications

Every file was changed by [`scripts/vendor-gcc.sh`](../../scripts/vendor-gcc.sh)
and carries a "Modified by cp-mac" notice at the top. The changes are
mechanical renames so the code no longer depends on libstdc++ internals:

- `#include`s of libstdc++-internal headers (`bits/c++config.h`, `debug/debug.h`,
  `bits/stl_*.h`, `tr1/*`, ...) point to cp-mac's compat headers instead
  (`include/cp-mac/libstdcxx_compat.h`, `include/cp-mac/rope_support.h`).
- `_GLIBCXX_*` macros and `__try` / `__catch` / `__throw_exception_again` are
  renamed to `__CPMAC_*` equivalents.
- libstdc++ internals used by rope (`std::_Destroy`, `std::__uninitialized_copy_n_a`,
  `__gnu_cxx::__alloc_traits`, gthread mutexes, ...) are replaced by
  standard-library-based versions in `__cpmac_rope::` and pthreads.
- `std::tr1::hash<crope>` becomes `std::hash<crope>`.
- Each file is marked `#pragma GCC system_header`.

No data structure logic was changed. To update to a newer GCC:

```sh
git clone --depth 1 --filter=blob:none --sparse https://github.com/gcc-mirror/gcc
git -C gcc sparse-checkout set --no-cone /libstdc++-v3/include/ext/ /COPYING3 /COPYING.RUNTIME
scripts/vendor-gcc.sh gcc
tests/run.sh
```
