package org.rephial.xyangband;

import java.util.Locale;

/** No Android SDK needed. Test stable setting IDs and locale fallback. */
public final class AppLanguageSelectionTest {
    private static int assertions = 0;

    private static void check(boolean condition, String message) {
        assertions++;
        if (!condition) throw new AssertionError(message);
    }

    private static void test(String setting, String device, boolean chinese,
                             String overrideTag) {
        Locale system = Locale.forLanguageTag(device);
        check(AppLanguageSelection.usesTraditionalChinese(setting, system) == chinese,
                setting + " / " + device + " language");
        Locale result = AppLanguageSelection.resourceOverride(setting, system);
        String actual = result == null ? null : result.toLanguageTag();
        check(overrideTag == null ? actual == null : overrideTag.equals(actual),
                setting + " / " + device + " resource override: " + actual);
    }

    public static void main(String[] args) {
        check("system".equals(AppLanguageSelection.normalize(null)), "null setting");
        check("system".equals(AppLanguageSelection.normalize("unknown")), "invalid setting");
        test("system", "en-US", false, null);
        test("system", "zh-TW", true, null);
        test("system", "zh-Hant-TW", true, null);
        test("system", "zh-Hant-HK", true, "zh-TW");
        test("system", "zh-CN", false, null);
        test("en", "zh-TW", false, "en");
        test("en", "en-US", false, "en");
        test("zh-TW", "en-US", true, "zh-TW");
        test("zh-TW", "zh-CN", true, "zh-TW");
        test("unknown", "zh-TW", true, null);
        test(null, "en-US", false, null);
        System.out.println("PASS: app-language selection (" + assertions + " assertions)");
    }
}
