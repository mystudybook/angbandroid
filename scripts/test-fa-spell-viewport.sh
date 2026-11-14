#!/bin/sh
# Verify the same spell menu geometry helper compiled by FAangband.
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
BUILD="$(mktemp -d "${TMPDIR:-/tmp}/fa-spell-viewport.XXXXXX")"
trap 'rm -rf "$BUILD"' 0 HUP INT TERM
"${CC:-cc}" -std=c99 -Wall -Wextra -Werror -pedantic -I "$ROOT" \
  "$ROOT/tests/native/test_fa_spell_viewport.c" -o "$BUILD/fa-spell-viewport"
"$BUILD/fa-spell-viewport"
