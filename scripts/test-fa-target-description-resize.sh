#!/bin/sh
# Compile the actual FAangband detail and input handlers under strict C99.
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
BUILD="$(mktemp -d "${TMPDIR:-/tmp}/fa-target-detail.XXXXXX")"
trap 'rm -rf "$BUILD"' 0 HUP INT TERM
python3 "$ROOT/tests/native/generate_fa_target_description_resize_host.py" \
    "$BUILD/detail.generated.c"
"${CC:-cc}" -std=c99 -Wall -Wextra -Werror -pedantic -I "$ROOT" \
    "$BUILD/detail.generated.c" -o "$BUILD/detail"
"$BUILD/detail"
