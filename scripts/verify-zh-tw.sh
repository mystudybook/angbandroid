#!/bin/sh
# Run offline localization checks and save a shareable combined log.
# Requires Python 3, a JDK (javac/java), and a host C compiler (cc).
# Optional --android also runs a Gradle/NDK debug APK build when the local
# Android SDK, NDK and Gradle wrapper are installed on the workstation.
# Neither mode runs emulator/physical-device gameplay acceptance tests.
set -u

ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
RUN_ANDROID=0
if [ "${1:-}" = "--android" ]; then
    RUN_ANDROID=1
    shift
fi
if [ "$#" -gt 1 ]; then
    printf 'Usage: sh scripts/verify-zh-tw.sh [--android] [report-file]\n' >&2
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
run_check "JDK 21 / Gradle 9.1 / AGP 9 build configuration" python3 "$ROOT/scripts/check_jdk21_build.py"
run_check "Android SDK/NDK preflight regression" python3 "$ROOT/tests/test_android_sdk_preflight.py"
run_check "Android string resources" python3 "$ROOT/scripts/check_zh_tw_resources.py"
run_check "Independent app-language source wiring" python3 "$ROOT/scripts/check_app_language_wiring.py"
run_check "Java Unicode terminal" sh "$ROOT/scripts/test-terminal-unicode.sh"
run_check "Native C UTF-8 and column-width helpers" sh "$ROOT/scripts/test-native-utf8.sh"
run_check "Legacy NPP bool headers across supported C standards" sh "$ROOT/scripts/test-native-bool-headers.sh"
run_check "Native shared-library symbol ownership" python3 "$ROOT/scripts/check_native_link_ownership.py"
run_check "Native/Java Unicode protocol wiring" python3 "$ROOT/scripts/check-native-unicode-contract.py"
run_check "Filtered native menu keyboard hint integration" python3 "$ROOT/tests/test_menu_key_hints_source.py"
run_check "Visible-page native menu shortcut consistency" python3 "$ROOT/tests/test_menu_visible_tags_source.py"
run_check "Finite invalid-row menu navigation" python3 "$ROOT/tests/test_menu_navigation_source.py"
run_check "Inventory and equipment UTF-8 display safety" python3 "$ROOT/tests/test_object_list_text_source.py"
run_check "Store columns on narrow/rotated terminals" python3 "$ROOT/tests/test_store_column_layout_source.py"
run_check "Store purchase cancel and stock-index safety" python3 "$ROOT/tests/test_store_stock_guard_source.py"
run_check "Scrolled store popups and empty-stock commands" python3 "$ROOT/tests/test_store_menu_flow_source.py"
run_check "Selected native menu action state" python3 "$ROOT/tests/test_menu_action_guard_source.py"
run_check "Native menu CJK column clipping" python3 "$ROOT/tests/test_menu_text_fit_source.py"
run_check "Compact menu layout on Android rotation" python3 "$ROOT/tests/test_menu_compact_layout_source.py"
run_check "Native blank-cell touch safety" python3 "$ROOT/tests/test_menu_hit_test_source.py"
run_check "Narrow native menu column safety" python3 "$ROOT/tests/test_menu_column_width_source.py"
run_check "Native Angband game zh-TW text isolation" python3 "$ROOT/scripts/check_native_game_localization.py"
run_check "Canonical translation coverage audit" python3 "$ROOT/scripts/audit-zh-tw-coverage.py"
run_check "Coverage audit JSON regression" python3 "$ROOT/tests/test_localization_coverage.py"

android_debug_build() {
    if [ ! -f "$ROOT/gradlew" ]; then
        printf 'Missing gradlew in the project root\n'
        return 1
    fi
    # Android Studio usually creates local.properties for the SDK path.
    # It is no longer required solely for the optional ABI_FILTERS property.
    (
        cd "$ROOT" || exit 1
        printf 'JAVA_HOME=%s\n' "${JAVA_HOME:-<not set>}"
        java -version || return 1
        javac -version || return 1
        sh ./gradlew --version || return 1
        if [ -n "${ABI_FILTERS:-}" ]; then
            printf 'Debug native ABIs (environment override): %s\n' "$ABI_FILTERS"
            sh ./gradlew --no-daemon --warning-mode=summary --stacktrace "-PABI_FILTERS=$ABI_FILTERS" :app:assembleDebug
        else
            printf 'Debug native ABIs: project default arm64-v8a (unless overridden)\n'
            sh ./gradlew --no-daemon --warning-mode=summary --stacktrace :app:assembleDebug
        fi
    )
}

if [ "$RUN_ANDROID" -eq 1 ]; then
    printf '\n===== Android SDK/NDK/CMake preflight =====\n' >> "$REPORT"
    if python3 "$ROOT/scripts/check_android_sdk.py" >> "$REPORT" 2>&1; then
        printf 'PASS: Android SDK/NDK/CMake preflight\n' >> "$REPORT"
        printf 'PASS: Android SDK/NDK/CMake preflight\n'
        run_check "Android Gradle/NDK debug APK build" android_debug_build
    else
        printf 'FAIL: Android SDK/NDK/CMake preflight\n' >> "$REPORT"
        printf 'FAIL: Android SDK/NDK/CMake preflight\n'
        printf 'SKIP: Android Gradle/NDK debug APK build (install SDK components first)\n' >> "$REPORT"
        failed=1
    fi
fi

printf '\nResult: %s\n' "$(if [ "$failed" -eq 0 ]; then echo PASS; else echo FAIL; fi)" >> "$REPORT"
printf 'Verification report: %s\n' "$REPORT"
exit "$failed"
