#!/bin/sh
# Check real FAangband melee and ghost translation pairs without Android build.
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
BUILD="$(mktemp -d "${TMPDIR:-/tmp}/fa-melee-ghost.XXXXXX")"
trap 'rm -rf "$BUILD"' 0 HUP INT TERM
"${CC:-cc}" -std=c99 -Wall -Wextra -Werror -pedantic -I "$ROOT" \
    "$ROOT/tests/native/test_faangband_melee_ghost_locale.c" \
    -o "$BUILD/fa-melee-ghost"
"$BUILD/fa-melee-ghost"
