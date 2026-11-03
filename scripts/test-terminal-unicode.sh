#!/bin/sh
# Offline JDK regression tests for terminal Unicode widths and text-cell logic.
# Android stubs are used only here; production classes are compiled unchanged.
set -eu

ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
BUILD_DIR="$(mktemp -d)"
trap 'rm -rf "$BUILD_DIR"' EXIT HUP INT TERM

javac -encoding UTF-8 -d "$BUILD_DIR" \
  "$ROOT/tests/stubs/android/util/Log.java" \
  "$ROOT/tests/stubs/org/rephial/xyangband/Preferences.java" \
  "$ROOT/tests/stubs/org/rephial/xyangband/TermView.java" \
  "$ROOT/app/src/main/java/org/rephial/xyangband/TerminalUnicode.java" \
  "$ROOT/app/src/main/java/org/rephial/xyangband/TermWindow.java" \
  "$ROOT/app/src/main/java/org/rephial/xyangband/MouseInputOptions.java" \
  "$ROOT/app/src/main/java/org/rephial/xyangband/GameTextLocale.java" \
  "$ROOT/app/src/main/java/org/rephial/xyangband/AppLanguageSelection.java" \
  "$ROOT/tests/java/org/rephial/xyangband/AppLanguageSelectionTest.java" \
  "$ROOT/tests/java/org/rephial/xyangband/GameTextLocaleTest.java" \
  "$ROOT/tests/java/org/rephial/xyangband/MouseInputOptionsTest.java" \
  "$ROOT/tests/java/org/rephial/xyangband/TerminalUnicodeTest.java" \
  "$ROOT/tests/java/org/rephial/xyangband/TermWindowUnicodeTest.java" \
  "$ROOT/tests/java/org/rephial/xyangband/TermWindowStressTest.java"

java -cp "$BUILD_DIR" org.rephial.xyangband.TerminalUnicodeTest
java -cp "$BUILD_DIR" org.rephial.xyangband.TermWindowUnicodeTest
java -cp "$BUILD_DIR" org.rephial.xyangband.TermWindowStressTest
java -cp "$BUILD_DIR" org.rephial.xyangband.MouseInputOptionsTest
java -cp "$BUILD_DIR" org.rephial.xyangband.GameTextLocaleTest
java -cp "$BUILD_DIR" org.rephial.xyangband.AppLanguageSelectionTest
