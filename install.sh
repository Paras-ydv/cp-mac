#!/bin/sh
# cp-mac installer
#
#   curl -fsSL https://raw.githubusercontent.com/Paras-ydv/cp-mac/main/install.sh | sh
#
# or, from a clone:   ./install.sh [--prefix DIR] [--uninstall]
#
# Installs headers into $PREFIX/include (default /usr/local/include, which
# Apple Clang searches out of the box on both Intel and Apple Silicon) and
# the cp-mac / cprun commands into $PREFIX/bin. Asks for your password only
# if that folder isn't writable.
set -eu

REPO="Paras-ydv/cp-mac"
REF="${CPMAC_REF:-main}"            # branch or tag to install from
PREFIX="${PREFIX:-/usr/local}"
ACTION=install

while [ $# -gt 0 ]; do
  case "$1" in
    --prefix) PREFIX="$2"; shift 2 ;;
    --prefix=*) PREFIX="${1#*=}"; shift ;;
    --uninstall) ACTION=uninstall; shift ;;
    --ref) REF="$2"; shift 2 ;;
    -h|--help)
      sed -n '2,12p' "$0" 2>/dev/null | sed 's/^# \{0,1\}//'
      exit 0 ;;
    *) echo "unknown option: $1" >&2; exit 2 ;;
  esac
done

INC="$PREFIX/include"
BIN="$PREFIX/bin"
MANIFEST="$INC/cp-mac/MANIFEST"

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

uninstall() {
  [ -f "$MANIFEST" ] || die "cp-mac doesn't appear to be installed in $PREFIX (no $MANIFEST)."
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

if [ "$ACTION" = uninstall ]; then uninstall; exit 0; fi

# ---- locate the files to install -------------------------------------------
SRC=""
HERE="$(cd "$(dirname "$0")" 2>/dev/null && pwd || true)"
if [ -n "$HERE" ] && [ -f "$HERE/include/bits/stdc++.h" ] && [ -f "$HERE/bin/cp-mac" ]; then
  SRC="$HERE"
else
  command -v curl >/dev/null 2>&1 || die "curl is required."
  TMP="$(mktemp -d)"
  trap 'rm -rf "$TMP"' EXIT
  case "$REF" in
    v[0-9]*) URL="https://github.com/$REPO/archive/refs/tags/$REF.tar.gz" ;;
    *)       URL="https://github.com/$REPO/archive/refs/heads/$REF.tar.gz" ;;
  esac
  say "Downloading cp-mac ($REF)"
  curl -fsSL "$URL" | tar -xz -C "$TMP" || die "Download failed: $URL"
  SRC="$(find "$TMP" -mindepth 1 -maxdepth 1 -type d | head -1)"
fi

VERSION="$(cat "$SRC/VERSION" 2>/dev/null || echo dev)"
say "Installing cp-mac $VERSION into $PREFIX"
need_sudo

# ---- move aside anything in the way that isn't ours -------------------------
backup() {  # backup <path>: rename a foreign file/dir to <path>.bak-<date>
  if [ -e "$1" ] && ! grep -rqs 'cp-mac' "$1" 2>/dev/null; then
    bak="$1.bak-$(date +%Y%m%d%H%M%S)"
    $SUDO mv "$1" "$bak"
    warn "Found an existing $(basename "$1") that isn't from cp-mac; moved it to $bak"
  fi
}
backup "$INC/bits/stdc++.h"
backup "$INC/bits/extc++.h"
backup "$INC/ext/pb_ds"
backup "$INC/ext/rope"
backup "$INC/ext/ropeimpl.h"

# Remove files from a previous cp-mac install first (handles renamed files).
if [ -f "$MANIFEST" ]; then
  while IFS= read -r f; do [ -n "$f" ] && $SUDO rm -f "$PREFIX/$f"; done < "$MANIFEST"
fi

# ---- copy ---------------------------------------------------------------------
LIST="$(mktemp)"
( cd "$SRC" && find include -type f ! -name '.*' | LC_ALL=C sort ) > "$LIST"
echo "bin/cp-mac" >> "$LIST"
echo "bin/cprun" >> "$LIST"

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
rm -f "$LIST"
$SUDO chmod -R a+rX "$INC/bits" "$INC/ext" "$INC/cp-mac"
ok "Installed $(wc -l < "$MANIFEST" | tr -d ' ') files"

# ---- self-test ------------------------------------------------------------------
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
if "$CXX_BIN" -std=c++17 ${EXTRA:+"$EXTRA"} "$T/t.cpp" -o "$T/t" 2>"$T/err" && [ "$("$T/t")" = ok ]; then
  ok "Self-test passed with $("$CXX_BIN" --version 2>/dev/null | head -1)"
  rm -rf "$T"
else
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
