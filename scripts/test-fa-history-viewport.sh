#!/bin/sh
# Test the exact FAangband history viewport helper with a host C99 compiler.
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
BUILD="$(mktemp -d "${TMPDIR:-/tmp}/fa-history-viewport.XXXXXX")"
trap 'rm -rf "$BUILD"' 0 HUP INT TERM
"${CC:-cc}" -std=c99 -Wall -Wextra -Werror -pedantic -I "$ROOT" \
    "$ROOT/tests/native/test_fa_history_viewport.c" \
    -o "$BUILD/fa-history-viewport"
"$BUILD/fa-history-viewport"
