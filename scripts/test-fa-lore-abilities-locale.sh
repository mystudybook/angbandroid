#!/bin/sh
# Compile the real FAangband ability language and UTF-8 display helper.
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
BUILD="$(mktemp -d "${TMPDIR:-/tmp}/fa-mon-abilities.XXXXXX")"
trap 'rm -rf "$BUILD"' 0 HUP INT TERM
"${CC:-cc}" -std=c99 -Wall -Wextra -Werror -pedantic -I "$ROOT" \
    "$ROOT/tests/native/test_faangband_lore_abilities_locale.c" \
    -o "$BUILD/fa-mon-abilities"
"$BUILD/fa-mon-abilities"
