#!/bin/sh
# Test production FAangband stacked monster subjects in strict host C99.
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
BUILD="$(mktemp -d "${TMPDIR:-/tmp}/fa-mon-msg.XXXXXX")"
trap 'rm -rf "$BUILD"' 0 HUP INT TERM
"${CC:-cc}" -std=c99 -Wall -Wextra -Werror -pedantic -I "$ROOT" \
    "$ROOT/tests/native/test_faangband_mon_msg_utf8.c" \
    -o "$BUILD/fa-mon-msg"
"$BUILD/fa-mon-msg"
