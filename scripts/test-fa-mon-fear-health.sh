#!/bin/sh
# Exercise the production FAangband fear-percentage helper on a host C99 compiler.
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
BUILD="$(mktemp -d "${TMPDIR:-/tmp}/fa-mon-fear.XXXXXX")"
trap 'rm -rf "$BUILD"' 0 HUP INT TERM
"${CC:-cc}" -std=c99 -Wall -Wextra -Werror -pedantic -I "$ROOT" \
    "$ROOT/tests/native/test_fa_mon_fear_health.c" \
    -o "$BUILD/fa-mon-fear"
"$BUILD/fa-mon-fear"
