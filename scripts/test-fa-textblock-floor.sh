#!/bin/sh
# Strict C99 tests for actual FAangband textblock and floor-list input functions.
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
BUILD="$(mktemp -d "${TMPDIR:-/tmp}/fa-textblock-floor.XXXXXX")"
trap 'rm -rf "$BUILD"' 0 HUP INT TERM
python3 "$ROOT/tests/native/generate_fa_textblock_pager_host.py" \
    "$BUILD/pager.generated.c"
"${CC:-cc}" -std=c99 -Wall -Wextra -Werror -pedantic -I "$ROOT" \
    "$BUILD/pager.generated.c" -o "$BUILD/pager"
"$BUILD/pager"
python3 "$ROOT/tests/native/generate_fa_floor_list_input_host.py" \
    "$BUILD/floor.generated.c"
"${CC:-cc}" -std=c99 -Wall -Wextra -Werror -pedantic -I "$ROOT" \
    "$BUILD/floor.generated.c" -o "$BUILD/floor"
"$BUILD/floor"
