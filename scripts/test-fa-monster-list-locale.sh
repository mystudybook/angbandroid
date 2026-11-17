#!/bin/sh
# FAangband monster list: real shared layout and CJK width helpers, strict C99.
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
BUILD="$(mktemp -d "${TMPDIR:-/tmp}/fa-mon-list.XXXXXX")"
trap 'rm -rf "$BUILD"' 0 HUP INT TERM
"${CC:-cc}" -std=c99 -Wall -Wextra -Werror -pedantic -I "$ROOT" \
    "$ROOT/tests/native/test_faangband_monster_list_locale.c" \
    -o "$BUILD/fa-mon-list"
"$BUILD/fa-mon-list"
