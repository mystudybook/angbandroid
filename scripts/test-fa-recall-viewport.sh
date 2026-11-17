#!/bin/sh
# Strict host C99 regression of the FAangband recall viewport.
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
BUILD="$(mktemp -d "${TMPDIR:-/tmp}/fa-recall-viewport.XXXXXX")"
trap 'rm -rf "$BUILD"' 0 HUP INT TERM
"${CC:-cc}" -std=c99 -Wall -Wextra -Werror -pedantic -I "$ROOT" \
 "$ROOT/tests/native/test_fa_recall_viewport.c" -o "$BUILD/recall-test"
"$BUILD/recall-test"
