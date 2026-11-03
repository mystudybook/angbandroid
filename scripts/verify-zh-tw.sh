#!/bin/sh
# Run offline localization checks and save a shareable combined log.
# Requires Python 3, a JDK (javac/java), and a host C compiler (cc).
# Optional --android also runs a Gradle/NDK debug APK build when the local
# Android SDK, NDK, Gradle wrapper and local.properties are configured.
# Neither mode runs emulator/physical-device gameplay acceptance tests.
set -u

ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
RUN_ANDROID=0
if [ "${1:-}" = "--android" ]; then
    RUN_ANDROID=1
    shift
fi
if [ "$#" -gt 1 ]; then
    printf 'Usage: sh scripts/verify-zh-tw.sh [--android] [report-file]\\n' >&2
    exit 2
fi
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

android_debug_build() {
    if [ ! -f "$ROOT/gradlew" ]; then
        printf 'Missing gradlew in the project root\\n'
        return 1
    fi
    if [ ! -f "$ROOT/local.properties" ]; then
        printf 'Missing local.properties (SDK path and ABI_FILTERS are required)\\n'
        return 1
    fi
    (
        cd "$ROOT" || exit 1
        sh ./gradlew --no-daemon :app:assembleDebug
    )
}

if [ "$RUN_ANDROID" -eq 1 ]; then
    run_check "Android Gradle/NDK debug APK build" android_debug_build
fi

printf '\nResult: %s\n' "$(if [ "$failed" -eq 0 ]; then echo PASS; else echo FAIL; fi)" >> "$REPORT"
printf 'Verification report: %s\n' "$REPORT"
exit "$failed"
