#!/bin/sh
# Exercise the production FAangband native terminal overwrite functions.
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
BUILD="$(mktemp -d "${TMPDIR:-/tmp}/fa-wide-overwrite.XXXXXX")"
trap 'rm -rf "$BUILD"' 0 HUP INT TERM
python3 "$ROOT/tests/native/generate_fa_terminal_wide_overwrite_host.py" \
    "$BUILD/terminal.generated.c"
"${CC:-cc}" -std=c99 -Wall -Wextra -Werror -pedantic -I "$ROOT" \
    "$BUILD/terminal.generated.c" -o "$BUILD/terminal"
"$BUILD/terminal"
