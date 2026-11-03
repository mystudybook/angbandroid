# Android Studio / Gradle 9 / JDK 21 build migration

The `master` branch uses **JDK 21** to run Gradle and compile Android source.

## Versions

| Component | Version | Notes |
|---|---|---|
| JDK to run Gradle | **21 recommended** | Gradle 9.1 supports JDK 21–25; project requires JDK 21+ |
| Compiler toolchain | **JDK 21** | `app/build.gradle` sets `JavaLanguageVersion.of(21)` |
| Java language and class-file target | **17** | Intentional: Android devices do not run a standard JDK 21 JVM |
| Gradle wrapper | **9.1.0** | Required by AGP 9.0; compatible with JDK 21 |
| Android Gradle Plugin | **9.0.1** | Requires Gradle 9.1.0 |
| compileSdk | **35** | Android SDK Platform 35 required |
| targetSdk | **34** | Unchanged in this build migration |
| minSdk | **22** | Unchanged |
| Android NDK | **28.2.13676358** (r28c) | Pinned to AGP 9's supported default; requires new native build validation |
| CMake | **3.10.2** | Project-pinned version; install via SDK Manager |
| Build Tools | **36.0.0** | Default required by AGP 9.0 |

**JDK vs. Android API:** JDK 21 here is the version of Java used by the developer machine to run Gradle and compile. The application's Java class-file compatibility remains at Java 17. This does **not** mean Android 5.1 devices must install Java 21, or that all JDK 21 library APIs are available in the app.

## On macOS

1. Install a JDK 21 distribution (Temurin, or any suitable JDK 21). In Android Studio select **Settings / Preferences → Build, Execution, Deployment → Build Tools → Gradle → Gradle JDK → JDK 21**.
2. Use an Android Studio version compatible with **AGP 9.0** (Panda 1 or newer), then open the **repository root** using **Open**, not the `app` subdirectory.
3. Install Android SDK Platform 35, SDK Build Tools 36.0.0, **Android NDK 28.2.13676358 (r28c)**, and CMake 3.10.2 in **Tools → SDK Manager**. Enable **Show Package Details** to select specific NDK and CMake versions.
4. **`ABI_FILTERS` is now optional.** Android Studio can create `local.properties` with the SDK path, and the debug build defaults to `arm64-v8a`. You do not need to add a custom `ABI_FILTERS` line just to open or sync the project. An optional `local.properties` looks like:

```properties
sdk.dir=/Users/YOUR_USER/Library/Android/sdk
```

If Android Studio has not created this file, you can instead set `ANDROID_HOME` to the SDK directory. For an Intel `x86_64` emulator, pass `-PABI_FILTERS=x86_64` to Gradle, or use `ABI_FILTERS=x86_64 sh scripts/verify-zh-tw.sh --android`. Multiple ABIs can be separated by commas or semicolons.

For existing installations, an old `ABI_FILTERS` value in `local.properties` is still accepted when no `-PABI_FILTERS` option is supplied. Android recommends keeping `local.properties` for SDK-specific paths, so new custom Gradle settings should normally use a project property or `~/.gradle/gradle.properties` instead.

5. In the Android Studio terminal, make sure JDK 21 is in use (the Android Studio Gradle JDK selection controls IDE builds; `JAVA_HOME` controls shell builds):

```bash
export JAVA_HOME="$(/usr/libexec/java_home -v 21)"
"$JAVA_HOME/bin/java" -version
sh ./gradlew --version
sh scripts/verify-zh-tw.sh --android
```

The optional `--android` flag invokes Gradle `:app:assembleDebug` after the offline checks. It records the active `java`, `javac`, Gradle runtime and a full build stacktrace when necessary in `zh-tw-validation.log`. When successful, the APK normally appears under `app/build/outputs/apk/debug/app-debug.apk`.

For an x86_64 emulator, run:

```bash
ABI_FILTERS=x86_64 sh scripts/verify-zh-tw.sh --android
```

You can also use **Build → Make Project** or **Run** in Android Studio, selecting an Android emulator or a physical test device. Prefer a fresh emulator or disposable data; this app uses the same `applicationId` as the upstream port and could encounter existing user save data if installed over another build.

## Compatibility changes already made

- Updated AGP to 9.0.1 and Gradle wrapper distribution to 9.1.0; switched `jcenter()` to `mavenCentral()`.
- Moved the namespace declaration into `app/build.gradle` and removed the old source-manifest `package` declaration required by AGP 8.
- Stopped pinning obsolete SDK Build Tools 30.0.3; AGP 9.0 requires Build Tools 36.0.0.
- Enabled AndroidX and preserved legacy final/transitive `R` reference behavior through tracked `gradle.properties`. AGP 9 requires the additional `android.enableAppCompileTimeRClass=false` compatibility option for old `switch (R.id...)` usage.
- Preserved existing `applicationId`, `targetSdk` and `minSdk`. Migrated the pinned NDK from 21.0.6113669 to **28.2.13676358** after a real AGP 9 build log showed the NDK was not configured.
- Removed the mandatory custom `local.properties` ABI field from Gradle Sync; default debug ABI is `arm64-v8a`, with a Gradle project property override and backwards-compatible optional legacy field.

## Gradle 9-specific migration details

- The Gradle Wrapper distribution URL, `gradle-wrapper.jar`, `gradlew`, and `gradlew.bat` have **all been updated** using the corresponding files from the official `gradle/gradle` `v9.1.0` GitHub tag. The Unix launcher is executable. `scripts/check_jdk21_build.py` verifies their Git blob IDs against the official source.
- There is no need to regenerate the Wrapper just to open this checkout. If you intentionally regenerate it later, run `./gradlew wrapper --gradle-version 9.1.0` twice to update both the distribution configuration and bootstrap files, then review and commit those files.

- AGP 9 introduces a new public DSL and enables built-in Kotlin by default. This project applies only `com.android.application` and has Java sources, so `android.builtInKotlin=false` avoids unnecessary Kotlin compilation. The new DSL remains enabled; do not use legacy `android.applicationVariants` APIs.
- AGP 9 defaults to non-final application compile-time `R` IDs. The project temporarily sets `android.enableAppCompileTimeRClass=false`, `android.nonFinalResIds=false` and `android.nonTransitiveRClass=false` to avoid breaking existing switch statements. Remove the compatibility flags only after the relevant UI code is modernized.
- `targetSdk 34`, `minSdk 22`, `compileSdk 35`, CMake 3.10.2, Java source/bytecode level 17 and `applicationId` remain unchanged. NDK is now pinned to r28c. A Gradle 9 build is not proof of Google Play submission eligibility.

## 2026-11-03 first macOS build log

The first uploaded `zh-tw-validation.log` reported **all offline localization tests passing**, and successfully ran JDK 21.0.2 and Gradle 9.1.0. APK configuration then failed with:

```text
NDK not configured. Download it with SDK manager.
Preferred NDK version is '28.2.13676358'.
```

This means no complete Android C/C++ or APK build ran. The same log also contains a warning about SDK repository XML version 4 being read by tools that recognize only version 3. Update **Android SDK Command-line Tools (latest)** in SDK Manager if this warning persists; it is a separate warning, not the fatal NDK error. The build now pins NDK r28c explicitly with modern Gradle 9 `property = value` declarations and checks local SDK packages **before** attempting the Gradle build. Deprecated AGP 9 compatibility flags (`android.nonFinalResIds=false`, `android.builtInKotlin=false`, and `android.enableAppCompileTimeRClass=false`) are left unchanged intentionally, because they are not the error in this log.

For an Android Studio user, select **Tools → SDK Manager → SDK Tools → Show Package Details**, and install NDK 28.2.13676358, CMake 3.10.2, and Build Tools 36.0.0 as needed. API 35 is under SDK Platforms. Or, if Command-line Tools are installed and the path is correct:

```bash
"$HOME/Library/Android/sdk/cmdline-tools/latest/bin/sdkmanager" --install \
  "ndk;28.2.13676358" "cmake;3.10.2.4988404" \
  "build-tools;36.0.0" "platforms;android-35"
```

Then rerun `sh scripts/verify-zh-tw.sh --android`. The first stage in the Android build now identifies which package is missing and skips Gradle until the missing tools have been installed. This cannot guarantee the APK builds successfully after those packages are present.

## Diagnosing failures

- **JDK outside supported range (below 21 or above 25):** configure Gradle JDK to 21 in Android Studio and/or set shell `JAVA_HOME` with `/usr/libexec/java_home -v 21`. Gradle 9.1 does not officially support running on JDK 26+.
- **Wrong emulator ABI / missing native library:** use `ABI_FILTERS=x86_64 sh scripts/verify-zh-tw.sh --android` for an x86_64 emulator. The default build targets ARM64; `ABI_FILTERS` is no longer required for Gradle Sync.
- **Missing SDK/NDK/CMake:** install the pinned packages via SDK Manager and accept SDK licenses.
- **Gradle Sync or dependency errors:** review the first `FAIL` stage and stack trace in `zh-tw-validation.log`, rather than upgrading Gradle/AGP arbitrarily.
- **`git pull` reports a local `gradle.properties` conflict:** the migration now tracks a non-secret `gradle.properties`. Back up your local untracked file before pulling and reapply any custom options through `~/.gradle/gradle.properties` as needed.
- **Android native C/C++ compile errors:** NDK r28c is a significant toolchain change; treat any warnings or compile errors as a separate native compatibility task. The NDK version change has not yet passed the full Gradle/NDK build.

## Verification boundary

A source-level migration review is not a successful Gradle Sync or APK build. Until `sh scripts/verify-zh-tw.sh --android` finishes successfully on a machine with Android SDK/NDK and JDK 21, compilation and on-device CJK/gameplay compatibility are still **unverified**. Shipping an APK to Google Play also requires a separate release-readiness review, including current target SDK requirements and bundled asset licenses.

Official references: https://developer.android.com/build/releases/agp-9-0-0-release-notes and https://docs.gradle.org/9.1.0/release-notes.html
