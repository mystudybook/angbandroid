package org.rephial.xyangband;

import java.util.Locale;

/** Run on a desktop JDK, independently of the Android SDK. */
public final class GameTextLocaleTest {
    private static int checked = 0;
    private static void check(String tag, boolean expected) {
        checked++;
        boolean result = GameTextLocale.usesTraditionalChinese(
                Locale.forLanguageTag(tag));
        if (result != expected) {
            throw new AssertionError(tag + ": expected " + expected
                    + ", got " + result);
        }
    }
    public static void main(String[] args) {
        check("zh-TW", true);
        check("zh-Hant", true);
        check("zh-Hant-TW", true);
        check("zh-Hant-HK", true);
        check("zh-Hans-CN", false);
        check("zh-CN", false);
        check("en-US", false);
        check("ja-JP", false);
        check("ko-KR", false);
        checked++;
        if (GameTextLocale.usesTraditionalChinese(null)) {
            throw new AssertionError("Null locale must use English fallback");
        }
        System.out.println("PASS: native text language selection (" + checked + " cases)");
    }
}
