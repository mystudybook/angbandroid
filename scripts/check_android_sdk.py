#!/usr/bin/env python3
"""Preflight Android SDK/NDK/CMake before invoking Gradle/AGP 9.

This checks the workstation only. It does not download packages or build
the APK, and it does not affect the Java/native source-only tests.
"""
from pathlib import Path
import os
import re
import sys

ROOT = Path(__file__).resolve().parents[1]
APP = ROOT / "app" / "build.gradle"
PROPS = ROOT / "local.properties"


def declared_version(content, name, default):
    match = re.search(r"^\s*" + re.escape(name)
                      + r"\s*=\s*[\"']?([\w.]+)", content, re.MULTILINE)
    return match.group(1) if match else default


def sdk_path():
    if PROPS.is_file():
        for line in PROPS.read_text(encoding="utf-8").splitlines():
            if line.strip().startswith("sdk.dir="):
                # local.properties escapes colons and backslashes, most often
                # when checked out on Windows.
                value = line.split("=", 1)[1].strip()
                value = value.replace("\\:", ":").replace("\\\\", "\\")
                if value:
                    return Path(value).expanduser()
    value = os.environ.get("ANDROID_HOME") or os.environ.get("ANDROID_SDK_ROOT")
    if value:
        return Path(value).expanduser()
    if sys.platform == "darwin":
        return Path.home() / "Library" / "Android" / "sdk"
    return None


def main():
    source = APP.read_text(encoding="utf-8")
    ndk = declared_version(source, "ndkVersion", "28.2.13676358")
    cmake = declared_version(source, "version", "3.10.2")
    api = declared_version(source, "compileSdk", "35")
    tools = "36.0.0"
    root = sdk_path()
    print(f"Android SDK location: {root or '<not configured>'}")
    print(f"Required packages: NDK {ndk}, CMake {cmake}, API {api}, Build Tools {tools}")
    if root is None or not root.is_dir():
        print("FAIL: Android SDK path is missing. In Android Studio open Tools > SDK Manager.")
        print("Set sdk.dir in root local.properties or set ANDROID_HOME.")
        return 1

    expected = (
        (f"NDK {ndk}", root / "ndk" / ndk / "source.properties",
         f"ndk;{ndk}"),
        (f"CMake {cmake}", root / "cmake" / cmake / "bin" / "cmake",
         f"cmake;{cmake}.4988404" if cmake == "3.10.2" else f"cmake;{cmake}"),
        (f"Android SDK Platform {api}", root / "platforms" /
         f"android-{api}" / "android.jar", f"platforms;android-{api}"),
        (f"SDK Build Tools {tools}", root / "build-tools" / tools /
         "source.properties", f"build-tools;{tools}"),
    )
    missing = []
    for name, path, package in expected:
        if path.exists():
            print(f"PASS: {name}")
        else:
            print(f"MISSING: {name} (expected {path})")
            missing.append(package)

    if missing:
        print("Install missing packages in Android Studio: "
              "Tools > SDK Manager > SDK Tools > Show Package Details.")
        manager = root / "cmdline-tools" / "latest" / "bin" / "sdkmanager"
        if manager.is_file():
            quoted = " ".join('"' + package + '"' for package in missing)
            print(f'Or via: "{manager}" --install {quoted}')
        print("FAIL: Android SDK/NDK preflight. No Gradle build attempted.")
        return 1
    print("PASS: Android SDK/NDK/CMake preflight (presence only)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
