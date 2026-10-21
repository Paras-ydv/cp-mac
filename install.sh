#!/bin/sh
# cp-mac installer
#
#   curl -fsSL https://raw.githubusercontent.com/Paras-ydv/cp-mac/main/install.sh | sh
#
# or, from a clone:   ./install.sh [--prefix DIR] [--ref REF] [--uninstall]
#
# Installs headers into $PREFIX/include (default /usr/local/include, which
# Apple Clang searches out of the box on both Intel and Apple Silicon) and
# the cp-mac / cprun commands into $PREFIX/bin. Asks for your password only
# if that folder isn't writable. Installs the latest release unless --ref
# (or CPMAC_REF) names a branch, tag or commit.
set -eu

# Everything runs from main() at the very end, so a download cut off halfway
# through `curl | sh` runs nothing instead of half an installer.

REPO="Paras-ydv/cp-mac"

usage() {
  cat <<'EOF'
cp-mac installer

  curl -fsSL https://raw.githubusercontent.com/Paras-ydv/cp-mac/main/install.sh | sh
  curl -fsSL https://raw.githubusercontent.com/Paras-ydv/cp-mac/main/install.sh | sh -s -- [options]
  ./install.sh [options]          (from a clone: installs that checkout)

Options:
  --prefix DIR   install into DIR/include and DIR/bin (default /usr/local)
  --ref REF      install this branch, tag or commit instead of the latest release
  --uninstall    remove cp-mac from the prefix
  -h, --help     show this help

Environment: CPMAC_PREFIX and CPMAC_REF work like --prefix and --ref.
EOF
}

if [ -t 1 ]; then B="$(printf '\033[1m')"; G="$(printf '\033[32m')"; R="$(printf '\033[31m')"; Y="$(printf '\033[33m')"; N="$(printf '\033[0m')"
else B=""; G=""; R=""; Y=""; N=""; fi
say()  { printf '%s==>%s %s\n' "$B" "$N" "$*"; }
ok()   { printf '%s✓%s %s\n' "$G" "$N" "$*"; }
warn() { printf '%s!%s %s\n' "$Y" "$N" "$*"; }
die()  { printf '%sx%s %s\n' "$R" "$N" "$*" >&2; exit 1; }

# Use sudo only if some folder we'll write into isn't writable by us.
# Checks the real target folders, not just $PREFIX: on Intel Macs /usr/local
# is root's but include/ and bin/ belong to Homebrew's user.
SUDO=""
writable() {  # writable <path>: nearest existing ancestor is writable
  d="$1"
  while [ ! -e "$d" ]; do d="$(dirname "$d")"; done
  [ -w "$d" ]
}
need_sudo() {
  for t in "$INC" "$BIN" "$INC/bits" "$INC/ext" "$INC/ext/pb_ds" "$INC/cp-mac"; do
    writable "$t" || SUDO="sudo"
  done
  if [ -f "$MANIFEST" ]; then  # folders holding files from a previous install
    while IFS= read -r f; do
      [ -n "$f" ] && { writable "$(dirname "$PREFIX/$f")" || SUDO="sudo"; }
    done < "$MANIFEST"
  fi
  if [ -n "$SUDO" ]; then
    command -v sudo >/dev/null 2>&1 || die "$PREFIX is not writable and sudo is not available. Try --prefix \$HOME/.local"
    say "Writing to $PREFIX needs administrator rights; you may be asked for your password."
  fi
}

# Homebrew's cp-mac shows up in $INC as symlinks: made by `cp-mac link` on
# Apple Silicon, or by Homebrew itself on Intel. Copying or deleting through
# them would change Homebrew's files, so refuse and explain instead.
check_no_brew_links() {
  for p in "$INC/bits" "$INC/ext" "$INC/cp-mac" "$INC/bits/stdc++.h" "$INC/bits/extc++.h" \
           "$INC/ext/pb_ds" "$INC/ext/rope" "$INC/ext/ropeimpl.h"; do
    [ -L "$p" ] || continue
    case "$(readlink "$p")" in
      *cp-mac*) die "$p is a link to Homebrew's cp-mac ($(readlink "$p")).
  Use one install method. To switch to this one, first run:
    cp-mac unlink && brew uninstall cp-mac" ;;
    esac
  done
}

uninstall() {
  [ -f "$MANIFEST" ] || die "cp-mac doesn't appear to be installed in $PREFIX (no $MANIFEST)."
  check_no_brew_links
  need_sudo
  say "Removing cp-mac from $PREFIX"
  failed=0
  while IFS= read -r f; do
    [ -z "$f" ] || $SUDO rm -f "$PREFIX/$f" || failed=1
  done < "$MANIFEST"
  [ "$failed" = 0 ] || die "Some files could not be removed (see above). The manifest is kept at $MANIFEST so you can retry."
  $SUDO rm -f "$MANIFEST" "$INC/cp-mac/VERSION"
  # remove now-empty directories we created
  for d in "$INC/ext/pb_ds" "$INC/cp-mac" "$INC/ext" "$INC/bits"; do
    [ -d "$d" ] && $SUDO find "$d" -depth -type d -empty -exec rmdir {} \; 2>/dev/null || true
  done
  ok "cp-mac uninstalled."
}

# The tag of the latest GitHub release (e.g. v1.0.0), or nothing if there is none.
latest_release() {
  u="$(curl -fsSIL -m 15 -o /dev/null -w '%{url_effective}' "https://github.com/$REPO/releases/latest" 2>/dev/null)" || return 0
  case "$u" in */releases/tag/*) echo "${u##*/releases/tag/}" ;; esac
}

# Move aside anything in the way that isn't ours.
backup() {  # backup <path>: rename a foreign file/dir to <path>.bak-<date>
  if [ -L "$1" ] || { [ -e "$1" ] && ! grep -rqs 'cp-mac' "$1" 2>/dev/null; }; then
    bak="$1.bak-$(date +%Y%m%d%H%M%S)"
    while [ -e "$bak" ] || [ -L "$bak" ]; do bak="$bak-1"; done
    $SUDO mv "$1" "$bak"
    warn "Found an existing $(basename "$1") that isn't from cp-mac; moved it to $bak"
  fi
}

main() {
  REF="${CPMAC_REF:-}"
  PREFIX="${CPMAC_PREFIX:-/usr/local}"
  ACTION=install

  while [ $# -gt 0 ]; do
    case "$1" in
      --prefix|--ref) [ $# -ge 2 ] || die "$1 needs a value"
                      if [ "$1" = --prefix ]; then PREFIX="$2"; else REF="$2"; fi; shift 2 ;;
      --prefix=*) PREFIX="${1#*=}"; shift ;;
      --ref=*) REF="${1#*=}"; shift ;;
      --uninstall) ACTION=uninstall; shift ;;
      -h|--help) usage; exit 0 ;;
      *) echo "unknown option: $1 (see --help)" >&2; exit 2 ;;
    esac
  done
  [ -n "$PREFIX" ] || die "--prefix is empty"
  case "$PREFIX" in /*) ;; *) PREFIX="$(pwd)/$PREFIX" ;; esac

  INC="$PREFIX/include"
  BIN="$PREFIX/bin"
  MANIFEST="$INC/cp-mac/MANIFEST"

  if [ "$ACTION" = uninstall ]; then uninstall; exit 0; fi

  # ---- locate the files to install -----------------------------------------
  # A local checkout is used only when this file itself was run from one
  # (./install.sh or make install). Under `curl | sh`, $0 is just "sh", and the
  # current directory must not decide what gets installed.
  SRC=""
  case "$0" in
    */install.sh|install.sh)
      if [ -z "$REF" ] && [ -f "$0" ]; then
        HERE="$(CDPATH='' cd -- "$(dirname -- "$0")" 2>/dev/null && pwd)" || HERE=""
        [ -n "$HERE" ] && [ -f "$HERE/include/bits/stdc++.h" ] && [ -f "$HERE/bin/cp-mac" ] && SRC="$HERE"
      fi ;;
  esac
  if [ -z "$SRC" ]; then
    command -v curl >/dev/null 2>&1 || die "curl is required."
    command -v tar >/dev/null 2>&1 || die "tar is required."
    [ -n "$REF" ] || REF="$(latest_release)"
    [ -n "$REF" ] || { REF=main; warn "Couldn't find a cp-mac release; installing the development version (main)."; }
    TMP="$(mktemp -d)"
    trap 'rm -rf "$TMP"' EXIT
    URL="https://github.com/$REPO/archive/$REF.tar.gz"   # works for branches, tags and commits
    say "Downloading cp-mac ($REF)"
    # Download to a file first: piping a failed download into tar can "succeed"
    # with nothing unpacked, and we must not touch an existing install then.
    curl -fsSL --retry 2 "$URL" -o "$TMP/src.tar.gz" || die "Download failed: $URL
  Check your internet connection and that '$REF' exists, then try again."
    tar -xzf "$TMP/src.tar.gz" -C "$TMP" || die "Could not unpack $URL"
    SRC="$(find "$TMP" -mindepth 1 -maxdepth 1 -type d | head -1)"
  fi
  [ -n "$SRC" ] && [ -f "$SRC/include/bits/stdc++.h" ] && [ -f "$SRC/bin/cp-mac" ] \
    || die "The downloaded files don't look like cp-mac (from ${URL:-$SRC}); nothing was changed."

  VERSION="$(cat "$SRC/VERSION" 2>/dev/null || echo dev)"
  say "Installing cp-mac $VERSION into $PREFIX"
  check_no_brew_links
  need_sudo

  backup "$INC/bits/stdc++.h"
  backup "$INC/bits/extc++.h"
  backup "$INC/ext/pb_ds"
  backup "$INC/ext/rope"
  backup "$INC/ext/ropeimpl.h"

  # Build the new file list before removing anything.
  LIST="$(mktemp)"
  trap 'rm -rf "${TMP:-}" "$LIST"' EXIT
  ( cd "$SRC" && find include -type f ! -name '.*' ) > "$LIST.unsorted" || die "Could not list files in $SRC"
  LC_ALL=C sort "$LIST.unsorted" > "$LIST"; rm -f "$LIST.unsorted"
  echo "bin/cp-mac" >> "$LIST"
  echo "bin/cprun" >> "$LIST"

  # Remove files from a previous cp-mac install first (handles renamed files).
  if [ -f "$MANIFEST" ]; then
    while IFS= read -r f; do [ -z "$f" ] || $SUDO rm -f "$PREFIX/$f"; done < "$MANIFEST"
  fi

  # ---- copy -------------------------------------------------------------------
  $SUDO mkdir -p "$INC" "$BIN"
  while IFS= read -r f; do
    case "$f" in
      bin/cprun) continue ;;   # symlink, created below
    esac
    $SUDO mkdir -p "$PREFIX/$(dirname "$f")"
    $SUDO cp "$SRC/$f" "$PREFIX/$f"
  done < "$LIST"
  $SUDO chmod 755 "$BIN/cp-mac"
  $SUDO ln -sf cp-mac "$BIN/cprun"
  $SUDO mkdir -p "$INC/cp-mac"
  $SUDO cp "$LIST" "$MANIFEST"
  echo "$VERSION" | $SUDO tee "$INC/cp-mac/VERSION" >/dev/null
  $SUDO chmod -R a+rX "$INC/bits" "$INC/ext" "$INC/cp-mac"
  ok "Installed $(wc -l < "$MANIFEST" | tr -d ' ') files"

  # ---- self-test --------------------------------------------------------------
  CXX_BIN="${CXX:-}"
  [ -z "$CXX_BIN" ] && for c in clang++ c++ g++; do command -v "$c" >/dev/null 2>&1 && { CXX_BIN="$c"; break; }; done
  if [ -z "$CXX_BIN" ]; then
    warn "No C++ compiler found. On a Mac, run: xcode-select --install"
    exit 0
  fi

  T="$(mktemp -d)"
  cat > "$T/t.cpp" <<'EOF'
#include <bits/stdc++.h>
#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/tree_policy.hpp>
#include <ext/rope>
using namespace std;
using namespace __gnu_pbds;
using namespace __gnu_cxx;
int main() {
  tree<int, null_type, less<int>, rb_tree_tag, tree_order_statistics_node_update> s;
  s.insert(5); s.insert(1);
  rope<int> r; r.push_back(7);
  cout << (*s.find_by_order(1) == 5 && r[0] == 7 && __gcd(12, 18) == 6 && __lg(8) == 3 ? "ok" : "bad") << "\n";
}
EOF
  EXTRA=""
  [ "$PREFIX" != /usr/local ] && EXTRA="-I$INC"
  # $CXX_BIN stays unquoted so CXX="ccache clang++" works.
  # shellcheck disable=SC2086
  if $CXX_BIN -std=c++17 ${EXTRA:+"$EXTRA"} "$T/t.cpp" -o "$T/t" 2>"$T/err" && [ "$("$T/t")" = ok ]; then
    # shellcheck disable=SC2086
    ok "Self-test passed with $($CXX_BIN --version 2>/dev/null | head -1)"
    rm -rf "$T"
  else
    sed 's/^/    /' "$T/err" | head -6
    rm -rf "$T"
    warn "Installed, but the self-test failed. Run 'cp-mac doctor' for a diagnosis."
    exit 1
  fi

  echo
  printf '%sDone!%s #include <bits/stdc++.h>, pb_ds and rope now work with clang++.\n' "$B" "$N"
  if [ "$PREFIX" != /usr/local ]; then
    echo "Because you used a custom prefix, compile with:  clang++ -I$INC file.cpp"
    case ":$PATH:" in *":$BIN:"*) ;; *) echo "Add $BIN to your PATH to use cprun." ;; esac
  fi
  echo "Compile and run a file with sane CP flags:  cprun main.cpp < input.txt"
  echo "Check your setup any time:                 cp-mac doctor"
}

main "$@"
