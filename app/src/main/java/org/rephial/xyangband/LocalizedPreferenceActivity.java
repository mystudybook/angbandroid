package org.rephial.xyangband;

import android.content.Context;
import android.preference.PreferenceActivity;

/** Legacy PreferenceActivity requires the same per-Activity language context. */
abstract class LocalizedPreferenceActivity extends PreferenceActivity {
    @Override
    protected void attachBaseContext(Context base) {
        super.attachBaseContext(AppLanguageContext.wrap(base));
    }
}
