#!/bin/sh
# Test live FAangband melee action dictionary offline with a host C99 compiler.
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
BUILD="$(mktemp -d "${TMPDIR:-/tmp}/fa-melee-actions.XXXXXX")"
trap 'rm -rf "$BUILD"' 0 HUP INT TERM
"${CC:-cc}" -std=c99 -Wall -Wextra -Werror -pedantic -I "$ROOT" \
    "$ROOT/tests/native/test_faangband_melee_actions_locale.c" \
    -o "$BUILD/fa-melee-actions"
"$BUILD/fa-melee-actions"
