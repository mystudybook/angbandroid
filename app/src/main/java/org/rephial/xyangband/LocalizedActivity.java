package org.rephial.xyangband;

import android.app.Activity;
import android.content.Context;

/** Applies the app's language setting before any Android UI resources inflate. */
abstract class LocalizedActivity extends Activity {
    @Override
    protected void attachBaseContext(Context base) {
        super.attachBaseContext(AppLanguageContext.wrap(base));
    }
}
