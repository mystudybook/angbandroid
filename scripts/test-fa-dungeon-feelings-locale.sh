#!/bin/sh
# Verify actual shared FA dungeon-feeling locale dictionary with strict C99.
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
BUILD="$(mktemp -d "${TMPDIR:-/tmp}/fa-feeling-locale.XXXXXX")"
trap 'rm -rf "$BUILD"' 0 HUP INT TERM
"${CC:-cc}" -std=c99 -Wall -Wextra -Werror -pedantic -I "$ROOT" \
    "$ROOT/tests/native/test_fa_dungeon_feelings_locale.c" \
    -o "$BUILD/fa-feeling-locale"
"$BUILD/fa-feeling-locale"
