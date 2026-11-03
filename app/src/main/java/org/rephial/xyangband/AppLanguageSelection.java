package org.rephial.xyangband;

import java.util.Locale;

/** Stable user-facing language options; no Android dependency for unit tests. */
final class AppLanguageSelection {
    static final String FOLLOW_SYSTEM = "system";
    static final String ENGLISH = "en";
    static final String TRADITIONAL_CHINESE = "zh-TW";

    private AppLanguageSelection() { }

    static String normalize(String saved) {
        if (ENGLISH.equals(saved) || TRADITIONAL_CHINESE.equals(saved)) {
            return saved;
        }
        return FOLLOW_SYSTEM;
    }

    static boolean usesTraditionalChinese(String saved, Locale deviceLocale) {
        String selected = normalize(saved);
        if (TRADITIONAL_CHINESE.equals(selected)) return true;
        if (ENGLISH.equals(selected)) return false;
        return GameTextLocale.usesTraditionalChinese(deviceLocale);
    }

    /** Return null when resources should simply follow the system configuration. */
    static Locale resourceOverride(String saved, Locale deviceLocale) {
        String selected = normalize(saved);
        if (TRADITIONAL_CHINESE.equals(selected)) return Locale.TAIWAN;
        if (ENGLISH.equals(selected)) return Locale.ENGLISH;
        // Android's zh-TW resources do not always match zh-Hant-HK directly.
        // Reuse those translations for a Traditional Chinese script locale.
        if (GameTextLocale.usesTraditionalChinese(deviceLocale)
                && (deviceLocale == null
                    || !"TW".equalsIgnoreCase(deviceLocale.getCountry()))) {
            return Locale.TAIWAN;
        }
        return null;
    }
}
