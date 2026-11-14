#!/bin/sh
# Compile the live FAangband-scoped lore dictionaries in strict host C99.
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
BUILD="$(mktemp -d "${TMPDIR:-/tmp}/fa-mon-lore248.XXXXXX")"
trap 'rm -rf "$BUILD"' 0 HUP INT TERM
"${CC:-cc}" -std=c99 -Wall -Wextra -Werror -pedantic -I "$ROOT" \
    "$ROOT/tests/native/test_faangband_lore_spells_attacks_locale.c" \
    -o "$BUILD/fa-mon-lore248"
"$BUILD/fa-mon-lore248"
