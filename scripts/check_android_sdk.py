#!/usr/bin/env python3
"""Preflight Android SDK/NDK and externally installed CMake for AGP 9.

CMake 4.1.2 can be found via cmake.dir in local.properties, the SDK's cmake
folder, or PATH. SDK Manager is NOT required to provide CMake 4.1.2.
No packages are downloaded and no Android APK is built by this script.
"""
from pathlib import Path
import os
import re
import shutil
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[1]
APP = ROOT / "app" / "build.gradle"
PROPS = ROOT / "local.properties"


def declared_version(content, name, default):
    match = re.search(r"^\s*" + re.escape(name)
                      + r"\s*=\s*[\"']?([\w.]+)", content, re.MULTILINE)
    return match.group(1) if match else default


def local_prop(name):
    if not PROPS.is_file():
        return None
    found = None
    for line in PROPS.read_text(encoding="utf-8").splitlines():
        if line.lstrip().startswith(("#", "!")) or "=" not in line:
            continue
        key, value = line.split("=", 1)
        if key.strip() != name:
            continue
        value = value.strip().replace("\\:", ":").replace("\\\\", "\\")
        found = value or None  # Java Properties: last declaration wins.
    return found


def sdk_path():
    value = local_prop("sdk.dir")
    if not value:
        value = os.environ.get("ANDROID_HOME") or os.environ.get("ANDROID_SDK_ROOT")
    if value:
        return Path(value).expanduser()
    if sys.platform == "darwin":
        return Path.home() / "Library" / "Android" / "sdk"
    return None


def cmake_executable(path):
    """Accept a CMake install root, its bin directory, or the executable."""
    p = Path(path).expanduser()
    if p.is_file() and p.name.lower() in ("cmake", "cmake.exe"):
        return p
    names = ("cmake.exe", "cmake") if os.name == "nt" else ("cmake",)
    for folder in (p / "bin", p):
        for name in names:
            candidate = folder / name
            if candidate.is_file():
                return candidate
    return p / "bin" / names[-1]


def cmake_version(executable):
    try:
        result = subprocess.run(
            [str(executable), "--version"], capture_output=True, text=True,
            timeout=8, check=False
        )
    except (OSError, subprocess.TimeoutExpired) as exc:
        return None, str(exc)
    line = result.stdout.splitlines()[0] if result.stdout else ""
    match = re.match(r"cmake version (\d+\.\d+\.\d+)(?:\b|$)", line)
    if result.returncode or not match:
        return None, (result.stderr.strip() or line or "unrecognized CMake output")
    return match.group(1), None


def find_cmake(root, version):
    """Return (path or None, diagnostics). Explicit cmake.dir takes priority."""
    diagnostics = []
    configured = local_prop("cmake.dir")
    if configured:
        path = Path(configured).expanduser()
        if not path.is_absolute():
            path = (ROOT / path).resolve()
        # AGP's cmake.dir refers to an installation directory with bin/cmake.
        # A direct executable path or bin/ directory may pass a loose
        # preflight but still fail in AGP, so require the actual install root.
        executable = path / "bin" / ("cmake.exe" if os.name == "nt" else "cmake")
        candidates = [("local.properties cmake.dir", executable)]
    else:
        candidates = []
        sdk_cmake = root / "cmake" / version
        if sdk_cmake.is_dir():
            candidates.append(("Android SDK", cmake_executable(sdk_cmake)))
        system_cmake = shutil.which("cmake")
        if system_cmake:
            candidates.append(("PATH", Path(system_cmake)))

    for origin, exe in candidates:
        actual, error = cmake_version(exe)
        if actual == version:
            diagnostics.append(f"PASS: CMake {version} from {origin} ({exe})")
            return exe, diagnostics
        if actual:
            diagnostics.append(
                f"MISMATCH: CMake {actual} from {origin} ({exe}); "
                f"project requires exactly {version}"
            )
        else:
            diagnostics.append(f"UNUSABLE: {origin} ({exe}): {error}")
        if configured:
            break

    if root.is_dir():
        other_root = root / "cmake"
        if other_root.is_dir():
            other = sorted(entry.name for entry in other_root.iterdir() if entry.is_dir())
            if other:
                diagnostics.append("SDK CMake directories: " + ", ".join(other))
    diagnostics.append(
        f"MISSING: CMake {version}. Install/use that exact version, "
        "then ensure cmake --version reports it."
    )
    diagnostics.append(
        "For Android Studio, put cmake.dir=/absolute/path/to/cmake/install "
        "in the git-ignored root local.properties; the folder must contain bin/cmake."
    )
    diagnostics.append(
        "Homebrew users can find that folder with brew --prefix cmake. "
        "No SDK Manager CMake 4.1.2 package is assumed."
    )
    return None, diagnostics


def main():
    source = APP.read_text(encoding="utf-8")
    ndk = declared_version(source, "ndkVersion", "28.2.13676358")
    cmake = declared_version(source, "version", "4.1.2")
    api = declared_version(source, "compileSdk", "35")
    tools = "36.0.0"
    root = sdk_path()
    print(f"Android SDK location: {root or '<not configured>'}")
    print(f"Required packages: NDK {ndk}, CMake {cmake}, API {api}, Build Tools {tools}")
    if root is None or not root.is_dir():
        print("FAIL: Android SDK path is missing. In Android Studio open Tools > SDK Manager.")
        print("Set sdk.dir in root local.properties or set ANDROID_HOME.")
        return 1

    packages = (
        (f"NDK {ndk}", root / "ndk" / ndk / "source.properties", f"ndk;{ndk}"),
        (f"Android SDK Platform {api}",
         root / "platforms" / f"android-{api}" / "android.jar",
         f"platforms;android-{api}"),
        (f"SDK Build Tools {tools}",
         root / "build-tools" / tools / "source.properties", f"build-tools;{tools}"),
    )
    missing = []
    for name, path, package in packages:
        if path.exists():
            print(f"PASS: {name}")
        else:
            print(f"MISSING: {name} (expected {path})")
            missing.append(package)

    executable, diagnostics = find_cmake(root, cmake)
    for message in diagnostics:
        print(message)
    if missing:
        print("Install missing Android SDK packages via Tools > SDK Manager.")
        manager = root / "cmdline-tools" / "latest" / "bin" / "sdkmanager"
        if manager.is_file():
            quoted = " ".join('"' + package + '"' for package in missing)
            print(f'Or via: "{manager}" --install {quoted}')
    if missing or executable is None:
        print("FAIL: Android SDK/NDK/CMake preflight; no Gradle build attempted.")
        return 1
    print("PASS: Android SDK/NDK/CMake preflight (installed tools, not APK compilation)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
