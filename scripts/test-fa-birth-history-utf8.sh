#!/bin/sh
# Build and execute native FA character-history UTF-8 regressions locally.
# This never starts GitHub Actions or builds an Android APK.
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
TMPDIR_TEST="$(mktemp -d "${TMPDIR:-/tmp}/fa-history-utf8.XXXXXX")"
trap 'rm -rf "$TMPDIR_TEST"' 0 HUP INT TERM
"${CC:-cc}" -std=c99 -Wall -Wextra -Werror \
    "$ROOT/tests/test_faangband_birth_history_utf8.c" \
    -o "$TMPDIR_TEST/fa-history-utf8"
"$TMPDIR_TEST/fa-history-utf8"
