#!/usr/bin/env python3
"""Offline contract checks for Android JDK 21 / Gradle 9.1 / AGP 9.0.1.

Static configuration checks only: this does NOT download dependencies,
run Gradle or compile an APK.
"""
from pathlib import Path
import hashlib
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

    def git_blob_matches(path, expected_sha):
        try:
            content = (ROOT / path).read_bytes()
        except OSError as exc:
            problems.append(f"{path}: {exc}")
            return False
        # Windows checkouts may normalize launcher line endings; Git's
        # underlying blob still uses LF for these two text files.
        if path in ("gradlew", "gradlew.bat"):
            content = content.replace(b"\r\n", b"\n")
        prefix = f"blob {len(content)}\0".encode("ascii")
        return hashlib.sha1(prefix + content).hexdigest() == expected_sha

    expected = {
        "AGP 9.0.1": "com.android.tools.build:gradle:9.0.1" in top,
        "Gradle 9.1.0": "gradle-9.1.0-bin.zip" in wrapper,
        "Wrapper distribution URL validated": "validateDistributionUrl=true" in wrapper,
        "Wrapper download timeout": "networkTimeout=10000" in wrapper,
        "Official Gradle 9.1.0 wrapper JAR":
            git_blob_matches("gradle/wrapper/gradle-wrapper.jar",
                             "8bdaf60c75ab801e22807dde59e12a8735a34077"),
        "Official Gradle 9.1.0 Unix launcher":
            git_blob_matches("gradlew",
                             "61b77d7f371ff8d139d0a518d103a6492dc59696"),
        "Official Gradle 9.1.0 Windows launcher":
            git_blob_matches("gradlew.bat",
                             "bd8a8c0556a94aa0c97aa4ddd44bd20756205bb1"),
        "JDK 21 required": "JavaVersion.current() < JavaVersion.VERSION_21" in settings,
        "Gradle 9 supported JDK max": "JavaVersion.current() > JavaVersion.VERSION_25" in settings,
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
        "Gradle 9 provider-based clean task":
            "tasks.register('clean', Delete)" in top
            and "rootProject.layout.buildDirectory" in top
            and "delete rootProject.buildDir" not in top,
        "Gradle properties use valid comment syntax":
            all(not line.lstrip().startswith("//") for line in props.splitlines()),
        "AndroidX enabled": "android.useAndroidX=true" in props,
        "Legacy resource switch compatibility": "android.nonFinalResIds=false" in props,
        "AGP 9 compile-time R compatibility": "android.enableAppCompileTimeRClass=false" in props,
        "No unused Kotlin compilation": "android.builtInKotlin=false" in props,
        "Legacy transitive R compatibility": "android.nonTransitiveRClass=false" in props,
        "Tracked Gradle defaults": "/gradle.properties" not in ignored,
        "Machine-specific SDK stays private": "/local.properties" in ignored,
        "Optional debug ABI from Gradle project property":
            "providers.gradleProperty('ABI_FILTERS').orNull" in app,
        "Optional legacy ABI from local.properties":
            "localFile.isFile()" in app
            and "localProperties.getProperty('ABI_FILTERS')" in app,
        "Default debug ABI is ARM64": "debugAbiSpec = 'arm64-v8a'" in app,
        "Valid native ABI filter checked": "supportedAbis.contains(it)" in app,
        "AGP 9 mutable ABI collection": "abiFilters.addAll(my_abi_filters)" in app
            and "abiFilters = []" not in app,
        "Legacy variant APIs not used":
            all(token not in app + top for token in (
                "applicationVariants", "libraryVariants", "variantFilter",
                "dexOptions", "compileOptions <<"
            )),
        "No separately applied Kotlin Gradle plugin":
            "org.jetbrains.kotlin.android" not in app + top
            and "kotlin-android" not in app + top,
        "No mandatory custom local.properties reader": "def local_props(key)" not in app,
        "Build verification logs active Java": "sh ./gradlew --version" in read("scripts/verify-zh-tw.sh"),
        "Optional ABI override in verification": '"-PABI_FILTERS=$ABI_FILTERS"' in read("scripts/verify-zh-tw.sh"),
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

    print(f"PASS: {len(expected)} Android JDK 21 / Gradle 9.1 / AGP 9.0.1 configuration checks")
    print("NOTE: Static checks only; run sh scripts/verify-zh-tw.sh --android to build")
    return 0


if __name__ == "__main__":
    sys.exit(main())
