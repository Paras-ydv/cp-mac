# Contributing to cp-mac

Thanks for helping! The most useful contribution is a **snippet of GCC-only code that still doesn't work on a Mac**: open an issue with it and the output of `cp-mac doctor`.

## Running the tests

```sh
make test                                   # tests against this checkout (adds -Iinclude)
CXX=g++-15 make test                        # with real GCC: must still use GCC's own headers
CXX="clang++ -stdlib=libc++" tests/run.sh   # on Linux, with libc++
tests/run.sh --installed                    # tests whatever is installed, without -I
```

`tests/run.sh` compiles every file in `tests/` for each C++ standard your compiler supports, with `-Wall -Wextra -Werror`, plus `-fno-exceptions`, debug-mode and precompiled-header builds. Each test prints `<name>: OK` when it passes.

Every bug fix comes with a test that fails without the fix. For header behaviour, compare with real GCC: the goal is that output matches what a judge running GCC prints.

## Where things live

- `include/bits/stdc++.h`, `include/bits/extc++.h`: the headers people include.
- `include/cp-mac/`: cp-mac's own support code. `prelude.h` must stay safe to include before any standard header.
- `include/ext/`: GCC's pb_ds and rope, **generated** by `scripts/vendor-gcc.sh`. Don't edit these by hand: change the script, then re-run it, so the fix survives the next import from GCC.
- `bin/cp-mac`: the CLI (`cprun` is a symlink to it). POSIX `sh`, no bash-isms: it must run under bash 3.2 (macOS `/bin/sh`), dash and busybox.
- `install.sh`: the installer. Also POSIX `sh`, and everything runs from `main()` so a truncated `curl | sh` does nothing.

## Updating pb_ds / rope from a newer GCC

```sh
git clone --depth 1 https://gcc.gnu.org/git/gcc.git /tmp/gcc
scripts/vendor-gcc.sh /tmp/gcc
make test
```

## Before opening a pull request

- `make test` passes with Apple Clang (and with `g++-NN` if you touched headers).
- `shellcheck -S warning -s sh install.sh bin/cp-mac tests/run.sh` is clean if you touched scripts.
- Commit messages: one short line saying what changed, e.g. `Fix rope fill constructor`.
- Files under `include/ext/` and `include/cp-mac/rope_support.h` are GPL-3.0-or-later with the GCC Runtime Library Exception; everything else is MIT. New files are MIT unless they're derived from GCC.

CI runs everything on real macOS (Apple Silicon and Intel), Linux and with Homebrew; a pull request needs it green.
