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
| Android NDK | **21.0.6113669** | Intentionally preserved until native build migration |
| CMake | **3.10.2** | Project-pinned version; install via SDK Manager |
| Build Tools | **36.0.0** | Default required by AGP 9.0 |

**JDK vs. Android API:** JDK 21 here is the version of Java used by the developer machine to run Gradle and compile. The application's Java class-file compatibility remains at Java 17. This does **not** mean Android 5.1 devices must install Java 21, or that all JDK 21 library APIs are available in the app.

## On macOS

1. Install a JDK 21 distribution (Temurin, or any suitable JDK 21). In Android Studio select **Settings / Preferences → Build, Execution, Deployment → Build Tools → Gradle → Gradle JDK → JDK 21**.
2. Open the **repository root** in Android Studio using **Open**, not the `app` subdirectory.
3. Install Android SDK Platform 35, SDK Build Tools 36.0.0, Android NDK 21.0.6113669, and CMake 3.10.2 in **Tools → SDK Manager**. Enable **Show Package Details** to select specific NDK and CMake versions.
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
- Preserved existing `applicationId`, `targetSdk`, `minSdk`, and NDK versions to keep behavior changes outside this migration.
- Removed the mandatory custom `local.properties` ABI field from Gradle Sync; default debug ABI is `arm64-v8a`, with a Gradle project property override and backwards-compatible optional legacy field.

## Gradle 9-specific migration details

- The Gradle wrapper URL now selects **9.1.0**, but `gradle-wrapper.jar`, `gradlew` and `gradlew.bat` may still contain the original wrapper bootstrap version. Gradle documents that older wrapper files generally bootstrap newer distributions correctly. **After a successful Gradle Sync, regenerate the wrapper files twice** to refresh both the scripts and JAR:

```bash
sh ./gradlew wrapper --gradle-version 9.1.0
sh ./gradlew wrapper --gradle-version 9.1.0
git status --short
```

  Commit those generated wrapper files to GitHub when the local build is working. Do not download arbitrary wrapper JARs.
- AGP 9 introduces a new public DSL and enables built-in Kotlin by default. This project applies only `com.android.application` and has Java sources, so `android.builtInKotlin=false` avoids unnecessary Kotlin compilation. The new DSL remains enabled; do not use legacy `android.applicationVariants` APIs.
- AGP 9 defaults to non-final application compile-time `R` IDs. The project temporarily sets `android.enableAppCompileTimeRClass=false`, `android.nonFinalResIds=false` and `android.nonTransitiveRClass=false` to avoid breaking existing switch statements. Remove the compatibility flags only after the relevant UI code is modernized.
- `targetSdk 34`, `minSdk 22`, `compileSdk 35`, NDK 21.0.6113669, CMake 3.10.2, Java language/bytecode level 17 and `applicationId` remain unchanged. A Gradle 9 build is not proof of Google Play submission eligibility.

## Diagnosing failures

- **JDK outside supported range (below 21 or above 25):** configure Gradle JDK to 21 in Android Studio and/or set shell `JAVA_HOME` with `/usr/libexec/java_home -v 21`. Gradle 9.1 does not officially support running on JDK 26+.
- **Wrong emulator ABI / missing native library:** use `ABI_FILTERS=x86_64 sh scripts/verify-zh-tw.sh --android` for an x86_64 emulator. The default build targets ARM64; `ABI_FILTERS` is no longer required for Gradle Sync.
- **Missing SDK/NDK/CMake:** install the pinned packages via SDK Manager and accept SDK licenses.
- **Gradle Sync or dependency errors:** review the first `FAIL` stage and stack trace in `zh-tw-validation.log`, rather than upgrading Gradle/AGP arbitrarily.
- **`git pull` reports a local `gradle.properties` conflict:** the migration now tracks a non-secret `gradle.properties`. Back up your local untracked file before pulling and reapply any custom options through `~/.gradle/gradle.properties` as needed.
- **Android native C/C++ compile errors:** the NDK remains old by design; fix those separately from the JDK/Gradle compatibility migration.

## Verification boundary

A source-level migration review is not a successful Gradle Sync or APK build. Until `sh scripts/verify-zh-tw.sh --android` finishes successfully on a machine with Android SDK/NDK and JDK 21, compilation and on-device CJK/gameplay compatibility are still **unverified**. Shipping an APK to Google Play also requires a separate release-readiness review, including current target SDK requirements and bundled asset licenses.

Official references: https://developer.android.com/build/releases/agp-9-0-0-release-notes and https://docs.gradle.org/9.1.0/release-notes.html
