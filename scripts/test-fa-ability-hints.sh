#!/bin/sh
# Compile the actual curses/droid.c Android shortcut methods on the host.
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
TMP="$(mktemp -d "${TMPDIR:-/tmp}/fa-ability-hints.XXXXXX")"
trap 'rm -rf "$TMP"' 0 HUP INT TERM
python3 "$ROOT/tests/native/generate_fa_ability_hint_host.py" "$TMP/hints.generated.c"
"${CC:-cc}" -std=c99 -Wall -Wextra -Werror -pedantic -I "$ROOT" \
    "$TMP/hints.generated.c" -o "$TMP/ability-hints"
"$TMP/ability-hints"
