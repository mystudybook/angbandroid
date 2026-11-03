#!/usr/bin/env python3
"""Self-contained tests for the Android SDK package preflight.

Uses fake SDK directories. Does not need Android Studio or an NDK installed.
"""
from contextlib import redirect_stdout
from pathlib import Path
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


def call():
    stream = io.StringIO()
    with redirect_stdout(stream):
        result = checker.main()
    return result, stream.getvalue()


def main():
    original_app, original_props = checker.APP, checker.PROPS
    try:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            sdk = root / "sdk"
            sdk.mkdir()
            build = root / "app" / "build.gradle"
            touch(build)
            build.write_text('''android {
    compileSdk = 35
    ndkVersion = "28.2.13676358"
    externalNativeBuild { cmake { version = "3.10.2" } }
}
''', encoding="utf-8")
            props = root / "local.properties"
            props.write_text(f"sdk.dir={sdk}\n", encoding="utf-8")
            checker.APP, checker.PROPS = build, props

            status, output = call()
            assert status == 1 and "MISSING: NDK 28.2.13676358" in output
            assert "cmake;3.10.2.4988404" not in output or "CMake" in output

            touch(sdk / "ndk" / "28.2.13676358" / "source.properties")
            touch(sdk / "cmake" / "3.10.2.4988404" / "bin" / "cmake")
            touch(sdk / "platforms" / "android-35" / "android.jar")
            touch(sdk / "build-tools" / "36.0.0" / "source.properties")

            status, output = call()
            assert status == 0, output
            for item in ("NDK 28.2.13676358", "CMake 3.10.2",
                         "Platform 35", "Build Tools 36.0.0"):
                assert "PASS:" in output and item in output, item

            (sdk / "ndk" / "28.2.13676358" / "source.properties").unlink()
            status, output = call()
            assert status == 1 and "MISSING: NDK 28.2.13676358" in output

        print("PASS: SDK/NDK r28c/CMake preflight diagnostics (3 scenarios)")
    finally:
        checker.APP, checker.PROPS = original_app, original_props


if __name__ == "__main__":
    main()
