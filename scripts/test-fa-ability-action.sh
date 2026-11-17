#!/bin/sh
# Compile real specialty event callback with host C99 mocks, no NDK or CI.
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
TEMP="$(mktemp -d "${TMPDIR:-/tmp}/fa-ability-action.XXXXXX")"
trap 'rm -rf "$TEMP"' 0 HUP INT TERM
python3 "$ROOT/tests/native/generate_fa_ability_action_host.py" \
    "$TEMP/action.generated.c"
"${CC:-cc}" -std=c99 -Wall -Wextra -Werror -pedantic \
    "$TEMP/action.generated.c" -o "$TEMP/action"
"$TEMP/action"
