#!/bin/sh
# Host C99-only ability browser geometry test; no Android SDK / NDK / CI.
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
TMP_TEST="$(mktemp -d "${TMPDIR:-/tmp}/fa-ability-layout.XXXXXX")"
trap 'rm -rf "$TMP_TEST"' 0 HUP INT TERM
"${CC:-cc}" -std=c99 -Wall -Wextra -Werror -pedantic \
    "$ROOT/tests/native/test_faangband_ability_layout.c" \
    -o "$TMP_TEST/fa-ability-layout"
"$TMP_TEST/fa-ability-layout"
