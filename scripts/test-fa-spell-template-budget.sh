#!/bin/sh
# Strict host C99 byte-budget checks of source-exact Chinese spell messages.
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
BUILD="$(mktemp -d "${TMPDIR:-/tmp}/fa-spell-budget.XXXXXX")"
trap 'rm -rf "$BUILD"' 0 HUP INT TERM
"${CC:-cc}" -std=c99 -Wall -Wextra -Werror -pedantic -I "$ROOT" \
    "$ROOT/tests/native/test_faangband_spell_template_budget.c" \
    -o "$BUILD/fa-spell-budget"
"$BUILD/fa-spell-budget"
