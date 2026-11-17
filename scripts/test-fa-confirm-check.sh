#!/bin/sh
# Strict host C99 tests: actual UTF-8 helper and extracted native confirmation.
# No Android SDK, NDK, GitHub Actions, or gameplay data required.
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
BUILD="$(mktemp -d "${TMPDIR:-/tmp}/fa-confirm-check.XXXXXX")"
trap 'rm -rf "$BUILD"' 0 HUP INT TERM
"${CC:-cc}" -std=c99 -Wall -Wextra -Werror -pedantic \
    "$ROOT/tests/native/test_faangband_confirm_prompt_utf8.c" \
    -o "$BUILD/confirm-prompt"
"$BUILD/confirm-prompt"
python3 "$ROOT/tests/native/generate_fa_confirm_check_host.py" \
    "$BUILD/confirm.generated.c"
"${CC:-cc}" -std=c99 -Wall -Wextra -Werror -pedantic -I "$ROOT" \
    "$BUILD/confirm.generated.c" -o "$BUILD/confirm-action"
"$BUILD/confirm-action"
