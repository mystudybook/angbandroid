#!/bin/sh
# Source-exact FAangband projection lash names and special ghost subjects.
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
BUILD="$(mktemp -d "${TMPDIR:-/tmp}/fa-lash-ghost.XXXXXX")"
trap 'rm -rf "$BUILD"' 0 HUP INT TERM
"${CC:-cc}" -std=c99 -Wall -Wextra -Werror -pedantic -I "$ROOT" \
    "$ROOT/tests/native/test_faangband_spell_lash_ghost_locale.c" \
    -o "$BUILD/fa-lash-ghost"
"$BUILD/fa-lash-ghost"
