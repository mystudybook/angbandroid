#!/bin/sh
# Compile isolated native localization contracts with the host C99 compiler.
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
TMP="$(mktemp -d)"
trap 'rm -rf "$TMP"' EXIT HUP INT TERM
"${CC:-cc}" -std=c99 -Wall -Wextra -Werror \
    "$ROOT/tests/native/test_activation_effects_locale.c" \
    -o "$TMP/test_activation_effects_locale"
"$TMP/test_activation_effects_locale"
echo "PASS: native activation, curse, and property locale smoke tests"
