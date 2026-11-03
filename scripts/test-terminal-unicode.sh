#!/bin/sh
# Pure-Java regression test for East Asian terminal width classification.
# Does not require the Android SDK or execute a Gradle build.
set -eu

ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
BUILD_DIR="$(mktemp -d)"
trap 'rm -rf "$BUILD_DIR"' EXIT HUP INT TERM

javac -encoding UTF-8 -d "$BUILD_DIR" \
  "$ROOT/app/src/main/java/org/rephial/xyangband/TerminalUnicode.java" \
  "$ROOT/tests/java/org/rephial/xyangband/TerminalUnicodeTest.java"

java -cp "$BUILD_DIR" org.rephial.xyangband.TerminalUnicodeTest
