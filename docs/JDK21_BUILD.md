# Android Studio / JDK 21 build migration

The `master` branch uses **JDK 21** to run Gradle and compile Android source.

## Versions

| Component | Version | Notes |
|---|---|---|
| JDK to run Gradle | **21** | Required by `settings.gradle`; select JDK 21 in Android Studio |
| Compiler toolchain | **JDK 21** | `app/build.gradle` sets `JavaLanguageVersion.of(21)` |
| Java language and class-file target | **17** | Intentional: Android devices do not run a standard JDK 21 JVM |
| Gradle wrapper | **8.11.1** | Compatible with JDK 21 |
| Android Gradle Plugin | **8.9.2** | Requires Gradle 8.11.1 |
| compileSdk | **35** | Android SDK Platform 35 required |
| targetSdk | **34** | Unchanged in this build migration |
| minSdk | **22** | Unchanged |
| Android NDK | **21.0.6113669** | Intentionally preserved until native build migration |
| CMake | **3.10.2** | Project-pinned version; install via SDK Manager |
| Build Tools | **35.0.0** | Default supplied by AGP 8.9 |

**JDK vs. Android API:** JDK 21 here is the version of Java used by the developer machine to run Gradle and compile. The application's Java class-file compatibility remains at Java 17. This does **not** mean Android 5.1 devices must install Java 21, or that all JDK 21 library APIs are available in the app.

## On macOS

1. Install a JDK 21 distribution (Temurin, or any suitable JDK 21). In Android Studio select **Settings / Preferences → Build, Execution, Deployment → Build Tools → Gradle → Gradle JDK → JDK 21**.
2. Open the **repository root** in Android Studio using **Open**, not the `app` subdirectory.
3. Install Android SDK Platform 35, SDK Build Tools 35.0.0, Android NDK 21.0.6113669, and CMake 3.10.2 in **Tools → SDK Manager**. Enable **Show Package Details** to select specific NDK and CMake versions.
4. Confirm that the root `local.properties` (not committed to Git) includes the actual SDK directory and the existing `ABI_FILTERS` setting:

```properties
sdk.dir=/Users/YOUR_USER/Library/Android/sdk
ABI_FILTERS=arm64-v8a
```

Use `ABI_FILTERS=x86_64` instead for an Intel x86_64 emulator; choose the ABI supported by the emulator or physical device.

5. In the Android Studio terminal, make sure JDK 21 is in use (the Android Studio Gradle JDK selection controls IDE builds; `JAVA_HOME` controls shell builds):

```bash
export JAVA_HOME="$(/usr/libexec/java_home -v 21)"
"$JAVA_HOME/bin/java" -version
sh ./gradlew --version
sh scripts/verify-zh-tw.sh --android
```

The optional `--android` flag invokes Gradle `:app:assembleDebug` after the offline checks. Its log is saved in `zh-tw-validation.log`. When successful, the APK normally appears under `app/build/outputs/apk/debug/app-debug.apk`.

You can also use **Build → Make Project** or **Run** in Android Studio, selecting an Android emulator or a physical test device. Prefer a fresh emulator or disposable data; this app uses the same `applicationId` as the upstream port and could encounter existing user save data if installed over another build.

## Compatibility changes already made

- Updated AGP and Gradle wrapper versions; switched `jcenter()` to `mavenCentral()`.
- Moved the namespace declaration into `app/build.gradle` and removed the old source-manifest `package` declaration required by AGP 8.
- Stopped pinning obsolete SDK Build Tools 30.0.3; AGP 8.9 selects Build Tools 35.0.0.
- Enabled AndroidX and preserved the legacy final/transitive `R` reference behavior through the tracked `gradle.properties`.
- Preserved existing `applicationId`, `targetSdk`, `minSdk`, and NDK versions to keep behavior changes outside this migration.

## Diagnosing failures

- **JDK version below 21:** configure Gradle JDK to 21 in Android Studio, and/or set shell `JAVA_HOME` with `/usr/libexec/java_home -v 21`.
- **Missing `ABI_FILTERS`:** update root `local.properties`; it is intentionally ignored by Git.
- **Missing SDK/NDK/CMake:** install the pinned packages via SDK Manager and accept SDK licenses.
- **Gradle Sync or dependency errors:** review the first `FAIL` stage and stack trace in `zh-tw-validation.log`, rather than upgrading Gradle/AGP arbitrarily.
- **`git pull` reports a local `gradle.properties` conflict:** the migration now tracks a non-secret `gradle.properties`. Back up your local untracked file before pulling and reapply any custom options through `~/.gradle/gradle.properties` as needed.
- **Android native C/C++ compile errors:** the NDK remains old by design; fix those separately from the JDK/Gradle compatibility migration.

## Verification boundary

A source-level migration review is not a successful Gradle Sync or APK build. Until `sh scripts/verify-zh-tw.sh --android` finishes successfully on a machine with Android SDK/NDK and JDK 21, compilation and on-device CJK/gameplay compatibility are still **unverified**. Shipping an APK to Google Play also requires a separate release-readiness review, including current target SDK requirements and bundled asset licenses.

Reference: Android AGP 8.9.0 release notes (Gradle 8.11.1 and SDK API 35): https://developer.android.com/build/releases/agp-8-9-0-release-notes
