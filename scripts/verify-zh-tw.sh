#!/bin/sh
# Run all offline localization checks and save a shareable combined log.
# Requires Python 3, a JDK (javac/java), and a host C compiler (cc).
# This is NOT an Android APK, emulator, or physical-device acceptance test.
set -u

ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
REPORT="${1:-$ROOT/zh-tw-validation.log}"
: > "$REPORT" || exit 1
failed=0

run_check() {
    label="$1"
    shift
    printf '\n===== %s =====\n' "$label" >> "$REPORT"
    if "$@" >> "$REPORT" 2>&1; then
        printf 'PASS: %s\n' "$label" >> "$REPORT"
        printf 'PASS: %s\n' "$label"
    else
        printf 'FAIL: %s\n' "$label" >> "$REPORT"
        printf 'FAIL: %s\n' "$label"
        failed=1
    fi
}

printf 'Angband zh-TW offline verification\n' >> "$REPORT"
printf 'Repository: %s\n' "$ROOT" >> "$REPORT"
run_check "Android string resources" python3 "$ROOT/scripts/check_zh_tw_resources.py"
run_check "Java Unicode terminal" sh "$ROOT/scripts/test-terminal-unicode.sh"
run_check "Native C UTF-8 and column-width helpers" sh "$ROOT/scripts/test-native-utf8.sh"
run_check "Native/Java Unicode protocol wiring" python3 "$ROOT/scripts/check-native-unicode-contract.py"

printf '\nResult: %s\n' "$(if [ "$failed" -eq 0 ]; then echo PASS; else echo FAIL; fi)" >> "$REPORT"
printf 'Verification report: %s\n' "$REPORT"
exit "$failed"
