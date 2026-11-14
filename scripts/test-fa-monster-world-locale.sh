#!/bin/sh
# Offline host C99 translation dictionary verification for monster/world text.
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
BUILD="$(mktemp -d "${TMPDIR:-/tmp}/fa-monster-world.XXXXXX")"
trap 'rm -rf "$BUILD"' 0 HUP INT TERM
"${CC:-cc}" -std=c99 -Wall -Wextra -Werror -pedantic -I "$ROOT" \
    "$ROOT/tests/native/test_fa_monster_world_locale.c" \
    -o "$BUILD/fa-monster-world"
"$BUILD/fa-monster-world"
