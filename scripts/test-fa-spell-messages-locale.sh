#!/bin/sh
# Strict host C99 test of all 221 production FAangband battle templates.
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
BUILD="$(mktemp -d "${TMPDIR:-/tmp}/fa-spell-message.XXXXXX")"
trap 'rm -rf "$BUILD"' 0 HUP INT TERM
"${CC:-cc}" -std=c99 -Wall -Wextra -Werror -pedantic -I "$ROOT" \
    "$ROOT/tests/native/test_faangband_spell_messages_locale.c" \
    -o "$BUILD/fa-spell-message"
"$BUILD/fa-spell-message"
