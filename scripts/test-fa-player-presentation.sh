#!/bin/sh
# Host C99 check: FAangband character sheet labels, no APK or Actions.
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
BUILD="$(mktemp -d "${TMPDIR:-/tmp}/fa-player-captions.XXXXXX")"
trap 'rm -rf "$BUILD"' 0 HUP INT TERM
"${CC:-cc}" -std=c99 -Wall -Wextra -Werror -pedantic -I "$ROOT" \
    "$ROOT/tests/native/test_fa_player_presentation.c" -o "$BUILD/player-captions"
"$BUILD/player-captions"
