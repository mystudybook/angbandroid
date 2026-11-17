#!/bin/sh
# Compile the real FAangband get_com_ex() with standalone terminal/input mocks.
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
TMP="$(mktemp -d "${TMPDIR:-/tmp}/fa-command-prompt.XXXXXX")"
trap 'rm -rf "$TMP"' 0 HUP INT TERM
python3 "$ROOT/tests/native/generate_fa_command_prompt_host.py" \
    "$TMP/command.generated.c"
"${CC:-cc}" -std=c99 -Wall -Wextra -Werror -pedantic -I "$ROOT" \
    "$TMP/command.generated.c" -o "$TMP/command"
"$TMP/command"
