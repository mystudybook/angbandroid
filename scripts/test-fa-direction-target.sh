#!/bin/sh
# Compile the real direction handler and target footer with host C99 mocks.
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
BUILD="$(mktemp -d "${TMPDIR:-/tmp}/fa-direction-target.XXXXXX")"
trap 'rm -rf "$BUILD"' 0 HUP INT TERM
python3 "$ROOT/tests/native/generate_fa_direction_input_host.py" \
    "$BUILD/direction.generated.c"
"${CC:-cc}" -std=c99 -Wall -Wextra -Werror -pedantic -I "$ROOT" \
    "$BUILD/direction.generated.c" -o "$BUILD/direction"
"$BUILD/direction"
python3 "$ROOT/tests/native/generate_fa_target_footer_host.py" \
    "$BUILD/target.generated.c"
"${CC:-cc}" -std=c99 -Wall -Wextra -Werror -pedantic -I "$ROOT" \
    "$BUILD/target.generated.c" -o "$BUILD/target"
"$BUILD/target"
