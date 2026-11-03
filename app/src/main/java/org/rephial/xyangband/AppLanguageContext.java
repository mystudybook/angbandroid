package org.rephial.xyangband;

import android.content.Context;
import android.content.res.Configuration;
import java.util.Locale;

/** Resource-context override for each Activity (without changing system Locale). */
final class AppLanguageContext {
    private AppLanguageContext() { }

    static String selected(Context context) {
        return AppLanguageSelection.normalize(context.getSharedPreferences(
                Preferences.NAME, Context.MODE_PRIVATE).getString(
                Preferences.KEY_APP_LANGUAGE, AppLanguageSelection.FOLLOW_SYSTEM));
    }

    static Context wrap(Context base) {
        Locale current = base.getResources().getConfiguration().locale;
        Locale override = AppLanguageSelection.resourceOverride(selected(base), current);
        if (override == null) return base;

        Configuration config = new Configuration(base.getResources().getConfiguration());
        config.setLocale(override);
        config.setLayoutDirection(override);
        return base.createConfigurationContext(config);
    }

    static boolean usesTraditionalChinese(Context context) {
        // Native game calls this at startup, even when the existing UI Activity
        // has not yet been recreated following a preference change.
        return AppLanguageSelection.usesTraditionalChinese(
                selected(context), Locale.getDefault());
    }
}
