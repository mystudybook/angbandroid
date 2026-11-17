#!/bin/sh
# Offline, strict host C99 verification of FAangband 57 melee action templates.
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
BUILD="$(mktemp -d "${TMPDIR:-/tmp}/fa-blow-action.XXXXXX")"
trap 'rm -rf "$BUILD"' 0 HUP INT TERM
"${CC:-cc}" -std=c99 -Wall -Wextra -Werror -pedantic -I "$ROOT" \
    "$ROOT/tests/native/test_faangband_blow_action_subject_locale.c" \
    -o "$BUILD/fa-blow-action"
"$BUILD/fa-blow-action"
