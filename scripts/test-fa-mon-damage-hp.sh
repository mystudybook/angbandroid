#!/bin/sh
# Test the exact production HP helper on a host C99 compiler.
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
BUILD="$(mktemp -d "${TMPDIR:-/tmp}/fa-mon-hp.XXXXXX")"
trap 'rm -rf "$BUILD"' 0 HUP INT TERM
"${CC:-cc}" -std=c99 -Wall -Wextra -Werror -pedantic -I "$ROOT" \
    "$ROOT/tests/native/test_fa_mon_damage_hp.c" -o "$BUILD/fa-mon-hp"
"$BUILD/fa-mon-hp"
