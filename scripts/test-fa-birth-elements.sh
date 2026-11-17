#!/bin/sh
# Verify the actual C99 FAangband display helper with native ELEM IDs.
# Does not need Android, NDK or GitHub Actions.
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
TMP_TEST="$(mktemp -d "${TMPDIR:-/tmp}/fa-birth-elements.XXXXXX")"
trap 'rm -rf "$TMP_TEST"' 0 HUP INT TERM
"${CC:-cc}" -std=c99 -Wall -Wextra -Werror \
    "$ROOT/tests/test_faangband_birth_elements.c" \
    -o "$TMP_TEST/fa-birth-elements"
"$TMP_TEST/fa-birth-elements"
