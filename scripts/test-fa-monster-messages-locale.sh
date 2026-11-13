#!/bin/sh
# Host-only C99 test of FAangband monster message and pain translations.
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
BUILD="$(mktemp -d "${TMPDIR:-/tmp}/fa-mon-msg-locale.XXXXXX")"
trap 'rm -rf "$BUILD"' 0 HUP INT TERM
"${CC:-cc}" -std=c99 -Wall -Wextra -Werror -pedantic -I "$ROOT" \
    "$ROOT/tests/native/test_faangband_monster_messages_locale.c" \
    -o "$BUILD/fa-mon-msg-locale"
"$BUILD/fa-mon-msg-locale"
