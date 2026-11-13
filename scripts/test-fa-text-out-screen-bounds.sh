#!/bin/sh
# Compile and run the real FAangband text_out_to_screen() with C99 mocks.
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
BUILD="$(mktemp -d "${TMPDIR:-/tmp}/fa-text-out-bounds.XXXXXX")"
trap 'rm -rf "$BUILD"' 0 HUP INT TERM
python3 "$ROOT/tests/native/generate_fa_text_out_screen_bounds_host.py" \
    "$BUILD/text-out.generated.c"
"${CC:-cc}" -std=c99 -Wall -Wextra -Werror -pedantic \
    "$BUILD/text-out.generated.c" -o "$BUILD/text-out"
"$BUILD/text-out"
