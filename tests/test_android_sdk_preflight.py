#!/usr/bin/env python3
"""Offline tests for CMake 4.1.2 and Android SDK preflight, with fake tools."""
from contextlib import redirect_stdout
from pathlib import Path
from unittest.mock import patch
import importlib.util
import io
import tempfile

SCRIPT = Path(__file__).resolve().parents[1] / "scripts" / "check_android_sdk.py"
spec = importlib.util.spec_from_file_location("check_android_sdk", SCRIPT)
checker = importlib.util.module_from_spec(spec)
spec.loader.exec_module(checker)


def touch(path):
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text("test", encoding="utf-8")


def fake_cmake(path, version):
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(
        "#!/bin/sh\nprintf 'cmake version " + version + "\\n'\n",
        encoding="utf-8"
    )
    path.chmod(0o755)


def run(which=None):
    output = io.StringIO()
    with patch.object(checker.shutil, "which", return_value=which):
        with redirect_stdout(output):
            status = checker.main()
    return status, output.getvalue()


def main():
    original_app, original_props = checker.APP, checker.PROPS
    checks = 0
    try:
        with tempfile.TemporaryDirectory() as folder:
            root = Path(folder)
            sdk = root / "sdk"
            sdk.mkdir()
            build = root / "app" / "build.gradle"
            touch(build)
            build.write_text(
                'android {\n  compileSdk = 35\n'
                '  ndkVersion = "28.2.13676358"\n'
                '  externalNativeBuild { cmake { version = "4.1.2" } }\n}\n',
                encoding="utf-8"
            )
            props = root / "local.properties"
            props.write_text(f"sdk.dir={sdk}\n", encoding="utf-8")
            checker.APP, checker.PROPS = build, props

            status, message = run()
            assert status == 1 and "MISSING: NDK 28.2.13676358" in message
            assert "MISSING: CMake 4.1.2" in message
            checks += 1

            # An unrelated SDK CMake directory must not satisfy the pin.
            fake_cmake(sdk / "cmake" / "3.22.1" / "bin" / "cmake", "3.22.1")
            status, message = run()
            assert status == 1 and "SDK CMake directories: 3.22.1" in message
            checks += 1

            touch(sdk / "ndk" / "28.2.13676358" / "source.properties")
            touch(sdk / "platforms" / "android-35" / "android.jar")
            touch(sdk / "build-tools" / "36.0.0" / "source.properties")
            path_cmake = root / "homebrew" / "bin" / "cmake"
            fake_cmake(path_cmake, "4.1.2")
            status, message = run(str(path_cmake))
            assert status == 0 and "PASS: CMake 4.1.2 from PATH" in message
            checks += 1

            # A deliberately wrong explicit cmake.dir must not be ignored
            # just because the right executable is available on PATH.
            props.write_text(
                f"sdk.dir={sdk}\ncmake.dir={sdk / 'cmake' / '3.22.1'}\n",
                encoding="utf-8"
            )
            status, message = run(str(path_cmake))
            assert status == 1 and "MISMATCH: CMake 3.22.1" in message
            checks += 1

            props.write_text(
                f"sdk.dir={sdk}\ncmake.dir={path_cmake.parent.parent}\n",
                encoding="utf-8"
            )
            status, message = run()
            assert status == 0
            assert "PASS: CMake 4.1.2 from local.properties cmake.dir" in message
            checks += 1

            props.write_text(f"sdk.dir={sdk}\n", encoding="utf-8")
            fake_cmake(sdk / "cmake" / "4.1.2" / "bin" / "cmake", "4.1.2")
            status, message = run()
            assert status == 0 and "PASS: CMake 4.1.2 from Android SDK" in message
            checks += 1

            # Explicit wrong cmake.dir takes priority over SDK and PATH.
            props.write_text(
                f"sdk.dir={sdk}\ncmake.dir={sdk / 'cmake' / '3.22.1'}\n",
                encoding="utf-8"
            )
            status, message = run(str(path_cmake))
            assert status == 1 and "MISMATCH: CMake 3.22.1" in message
            checks += 1

        print(f"PASS: CMake 4.1.2 / NDK r28c preflight ({checks} scenarios)")
    finally:
        checker.APP, checker.PROPS = original_app, original_props


if __name__ == "__main__":
    main()
