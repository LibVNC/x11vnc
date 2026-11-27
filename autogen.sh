#!/bin/sh

srcdir=$(dirname "$0")
test -z "$srcdir" && srcdir=.

ORIGDIR=$(pwd)
cd "$srcdir"

test -d m4 || mkdir m4

if ! command -v autoreconf >/dev/null 2>&1; then
  for prefix in "${BREW_PREFIX:-}" /opt/homebrew /usr/local; do
    if [ -n "$prefix" ] && [ -x "$prefix/bin/autoreconf" ]; then
      PATH="$prefix/bin:$PATH"
      export PATH
      break
    fi
  done
fi

for tool in autoreconf aclocal automake; do
  if ! command -v "$tool" >/dev/null 2>&1; then
    echo "$tool not found; install with: brew install autoconf automake libtool pkg-config" >&2
    exit 1
  fi
done
if ! command -v libtoolize >/dev/null 2>&1 && ! command -v glibtoolize >/dev/null 2>&1; then
  echo "libtoolize/glibtoolize not found; install with: brew install autoconf automake libtool pkg-config" >&2
  exit 1
fi

need_autoreconf=0
if [ ! -x "$srcdir/configure" ]; then
  need_autoreconf=1
else
  for f in "$srcdir/configure.ac" "$srcdir/Makefile.am" \
           "$srcdir/src/Makefile.am" "$srcdir/misc/Makefile.am" \
           "$srcdir/misc/turbovnc/Makefile.am"; do
    if [ ! -f "$f" ]; then
      continue
    fi
    generated=""
    case "$f" in
      */Makefile.am) generated=$(dirname "$f")/Makefile.in ;;
    esac
    if [ "$f" -nt "$srcdir/configure" ]; then
      need_autoreconf=1
    fi
    if [ -n "$generated" ] &&
       { [ ! -f "$generated" ] || [ "$f" -nt "$generated" ]; }; then
      need_autoreconf=1
    fi
  done
  for m in "$srcdir"/m4/*.m4; do
    if [ -f "$m" ] && [ "$m" -nt "$srcdir/configure" ]; then
      need_autoreconf=1
    fi
  done
fi
if [ "$need_autoreconf" -eq 1 ]; then
  autoreconf -v --install || exit 1
fi
cd "$ORIGDIR" || exit $?

"$srcdir"/configure "$@"
