#!/bin/sh

srcdir=$(dirname "$0")
test -z "$srcdir" && srcdir=.

ORIGDIR=$(pwd)
cd "$srcdir"

test -d m4 || mkdir m4
autoreconf -v --install || exit 1
cd "$ORIGDIR" || exit $?

"$srcdir"/configure "$@"
