#!/bin/sh
# Compile bilingual FAangband item prompts from the production UI dictionary.
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
BUILD="$(mktemp -d "${TMPDIR:-/tmp}/fa-item-locale.XXXXXX")"
trap 'rm -rf "$BUILD"' 0 HUP INT TERM
"${CC:-cc}" -std=c99 -Wall -Wextra -Werror -pedantic -I "$ROOT" \
    "$ROOT/tests/native/test_fa_item_command_translation.c" \
    -o "$BUILD/fa-item-locale"
"$BUILD/fa-item-locale"
