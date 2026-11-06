#!/bin/sh
# Offline quantity slider edge cases; no Android SDK, Gradle or Actions.
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
BUILD_DIR="$(mktemp -d)"
trap 'rm -rf "$BUILD_DIR"' EXIT HUP INT TERM
javac -encoding UTF-8 -d "$BUILD_DIR" \
    "$ROOT/app/src/main/java/org/rephial/xyangband/QuantitySliderMath.java" \
    "$ROOT/app/src/main/java/org/rephial/xyangband/QuantityInputSequence.java" \
    "$ROOT/app/src/main/java/org/rephial/xyangband/QuantityPromptGate.java" \
    "$ROOT/tests/java/org/rephial/xyangband/QuantitySliderMathTest.java" \
    "$ROOT/tests/java/org/rephial/xyangband/QuantityInputSequenceTest.java" \
    "$ROOT/tests/java/org/rephial/xyangband/QuantityPromptGateTest.java"
java -cp "$BUILD_DIR" org.rephial.xyangband.QuantitySliderMathTest
java -cp "$BUILD_DIR" org.rephial.xyangband.QuantityInputSequenceTest
java -cp "$BUILD_DIR" org.rephial.xyangband.QuantityPromptGateTest
