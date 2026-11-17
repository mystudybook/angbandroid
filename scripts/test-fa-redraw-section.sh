#!/bin/sh
# Run the actual FAangband partial redraw code with a strict C99 grid mock.
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
BUILD="$(mktemp -d "${TMPDIR:-/tmp}/fa-redraw-section.XXXXXX")"
trap 'rm -rf "$BUILD"' 0 HUP INT TERM
python3 "$ROOT/tests/native/generate_fa_redraw_section_host.py" \
    "$BUILD/redraw.generated.c"
"${CC:-cc}" -std=c99 -Wall -Wextra -Werror -pedantic -I "$ROOT" \
    "$BUILD/redraw.generated.c" -o "$BUILD/redraw"
"$BUILD/redraw"
