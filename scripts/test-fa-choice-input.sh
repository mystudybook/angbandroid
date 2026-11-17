#!/bin/sh
# Compile the real FAangband get_char callback with terminal/input mocks.
# No Android SDK/NDK or GitHub Actions are used.
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
BUILD="$(mktemp -d "${TMPDIR:-/tmp}/fa-choice-input.XXXXXX")"
trap 'rm -rf "$BUILD"' 0 HUP INT TERM
"${CC:-cc}" -std=c99 -Wall -Wextra -Werror -pedantic \
    "$ROOT/tests/native/test_faangband_choice_prompt_utf8.c" \
    -o "$BUILD/choice-prompt"
"$BUILD/choice-prompt"
python3 "$ROOT/tests/native/generate_fa_choice_input_host.py" \
    "$BUILD/choice.generated.c"
"${CC:-cc}" -std=c99 -Wall -Wextra -Werror -pedantic -I "$ROOT" \
    "$BUILD/choice.generated.c" -o "$BUILD/choice-input"
"$BUILD/choice-input"
