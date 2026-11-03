#!/usr/bin/env python3
"""Offline contract checks for the Android JDK 21 / AGP 8.9 migration.

Static configuration checks only: this does NOT download dependencies,
run Gradle or compile an APK.
"""
from pathlib import Path
import re
import sys
import xml.etree.ElementTree as ET

ROOT = Path(__file__).resolve().parents[1]


def main():
    problems = []

    def read(path):
        file = ROOT / path
        try:
            return file.read_text(encoding="utf-8")
        except OSError as exc:
            problems.append(f"{path}: {exc}")
            return ""

    top = read("build.gradle")
    app = read("app/build.gradle")
    wrapper = read("gradle/wrapper/gradle-wrapper.properties")
    settings = read("settings.gradle")
    props = read("gradle.properties")
    ignored = read(".gitignore")
    manifest_path = ROOT / "app/src/main/AndroidManifest.xml"

    expected = {
        "AGP 8.9.2": "com.android.tools.build:gradle:8.9.2" in top,
        "Gradle 8.11.1": "gradle-8.11.1-bin.zip" in wrapper,
        "JDK 21 required": "JavaVersion.current() < JavaVersion.VERSION_21" in settings,
        "JDK 21 compiler toolchain": "JavaLanguageVersion.of(21)" in app,
        "Java 17 Android bytecode": "sourceCompatibility JavaVersion.VERSION_17" in app
            and "targetCompatibility JavaVersion.VERSION_17" in app,
        "Android namespace": 'namespace "org.rephial.xyangband"' in app,
        "Android compile SDK 35": bool(re.search(r"\bcompileSdk\s+35\b", app)),
        "Android applicationId unchanged": 'applicationId "org.rephial.xyangband"' in app,
        "Target SDK unchanged": bool(re.search(r"\btargetSdk\s+34\b", app)),
        "Min SDK unchanged": bool(re.search(r"\bminSdkVersion\s+22\b", app)),
        "NDK unchanged": 'ndkVersion "21.0.6113669"' in app,
        "Maven Central repositories": top.count("mavenCentral()") == 2
            and "jcenter()" not in top,
        "AndroidX enabled": "android.useAndroidX=true" in props,
        "Legacy resource switch compatibility": "android.nonFinalResIds=false" in props,
        "Legacy transitive R compatibility": "android.nonTransitiveRClass=false" in props,
        "Tracked Gradle defaults": "/gradle.properties" not in ignored,
        "Machine-specific SDK stays private": "/local.properties" in ignored,
    }

    try:
        manifest = ET.parse(manifest_path).getroot()
        expected["Manifest namespace migrated"] = "package" not in manifest.attrib
    except (ET.ParseError, OSError) as exc:
        problems.append(f"Manifest: {exc}")

    for label, ok in expected.items():
        if not ok:
            problems.append(f"Configuration mismatch: {label}")

    if problems:
        for problem in problems:
            print("FAIL:", problem)
        return 1

    print(f"PASS: {len(expected)} Android JDK 21 / AGP 8.9 configuration checks")
    print("NOTE: Static checks only; run sh scripts/verify-zh-tw.sh --android to build")
    return 0


if __name__ == "__main__":
    sys.exit(main())
