#!/bin/sh
# Independent FAangband C99 PF/OF name translation test.
# No Android SDK, NDK, APK or GitHub Actions invocation.
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
TMP_TEST="$(mktemp -d "${TMPDIR:-/tmp}/fa-birth-labels.XXXXXX")"
trap 'rm -rf "$TMP_TEST"' 0 HUP INT TERM
"${CC:-cc}" -std=c99 -Wall -Wextra -Werror \
    "$ROOT/tests/test_faangband_birth_ability_labels.c" \
    -o "$TMP_TEST/fa-birth-labels"
"$TMP_TEST/fa-birth-labels"
