#!/bin/sh
# Regression for NPP054/NPP710 boolean definitions and pre-included stdbool.h.
# Uses only the host C compiler. This does not build an Android APK.
set -eu

ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
BUILD_DIR="$(mktemp -d)"
trap 'rm -rf "$BUILD_DIR"' EXIT HUP INT TERM
CC="${CC:-cc}"
checks=0

for standard in c99 c11 c17 c2x; do
    # Test available standards; C99 is required, newer modes are optional.
    if ! printf 'int main(void) { return 0; }\n' |
        "$CC" "-std=$standard" -fsyntax-only -x c - >/dev/null 2>&1; then
        if [ "$standard" = c99 ]; then
            echo "FAIL: host C compiler does not support C99" >&2
            exit 1
        fi
        echo "SKIP: $standard not supported by $CC"
        continue
    fi

    for variant in npp054 npp710; do
        for mode in direct preincluded; do
            if [ "$mode" = preincluded ]; then
                include_flag=-DSTDBOOL_FIRST
            else
                include_flag=-DNO_STDBOOL_FIRST
            fi

            "$CC" "-std=$standard" -Wall -Wextra -Werror -pedantic \
                "$include_flag" \
                -I"$ROOT/app/src/main/cpp/$variant" \
                "$ROOT/tests/native/test_legacy_bool.c" \
                -o "$BUILD_DIR/test_legacy_bool"
            "$BUILD_DIR/test_legacy_bool"
            echo "PASS: $variant $standard (stdbool $mode)"
            checks=$((checks + 1))
        done
    done
done
echo "PASS: $checks legacy NPP bool header cases"
