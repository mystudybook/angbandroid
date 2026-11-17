#!/bin/sh
# FAangband: race-specific spell overrides and UTF-8 subjects (host C99).
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
BUILD="$(mktemp -d "${TMPDIR:-/tmp}/fa-alt-utf8.XXXXXX")"
trap 'rm -rf "$BUILD"' 0 HUP INT TERM
"${CC:-cc}" -std=c99 -Wall -Wextra -Werror -pedantic -I "$ROOT" \
    "$ROOT/tests/native/test_faangband_spell_alternates_utf8.c" \
    -o "$BUILD/fa-alt-utf8"
"$BUILD/fa-alt-utf8"
