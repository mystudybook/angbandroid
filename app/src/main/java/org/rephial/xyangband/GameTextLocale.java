package org.rephial.xyangband;

import java.util.Locale;

/**
 * Language selection for text rendered by native Angband.
 * This is separate from persistent game command and save identifiers.
 */
final class GameTextLocale {
    private GameTextLocale() { }

    static boolean usesTraditionalChinese(Locale locale) {
        if (locale == null || !"zh".equalsIgnoreCase(locale.getLanguage())) {
            return false;
        }
        String region = locale.getCountry();
        String script = locale.getScript();
        return "TW".equalsIgnoreCase(region)
                || "Hant".equalsIgnoreCase(script);
    }
}
