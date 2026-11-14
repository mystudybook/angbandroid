#!/bin/sh
# Host-only strict C99 test of 30 production monster-lore UI labels.
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
BUILD="$(mktemp -d "${TMPDIR:-/tmp}/fa-lore-display.XXXXXX")"
trap 'rm -rf "$BUILD"' 0 HUP INT TERM
"${CC:-cc}" -std=c99 -Wall -Wextra -Werror -pedantic -I "$ROOT" \
    "$ROOT/tests/native/test_faangband_lore_display_locale.c" \
    -o "$BUILD/fa-lore-display"
"$BUILD/fa-lore-display"
