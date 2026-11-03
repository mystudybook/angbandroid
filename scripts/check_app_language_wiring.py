#!/usr/bin/env python3
"""Static source checks for the independent (non-destructive) language setting.

Does not replace Android locale/resource or native gameplay device tests.
"""
from pathlib import Path
import xml.etree.ElementTree as ET

ROOT = Path(__file__).resolve().parents[1]
JAVA = ROOT / "app/src/main/java/org/rephial/xyangband"
RES = ROOT / "app/src/main/res"
checks = 0


def require(path, fragment):
    global checks
    content = path.read_text(encoding="utf-8")
    if fragment not in content:
        raise AssertionError(f"{path.relative_to(ROOT)} missing {fragment!r}")
    checks += 1


for name in ("AngbandActivity", "GameActivity", "HelpActivity",
             "ProfilesActivity", "ProfileAddActivity", "StorageActivity"):
    require(JAVA / f"{name}.java", f"class {name} extends LocalizedActivity")
require(JAVA / "PreferencesActivity.java", "extends LocalizedPreferenceActivity")
require(JAVA / "LocalizedActivity.java", "AppLanguageContext.wrap(base)")
require(JAVA / "LocalizedPreferenceActivity.java", "AppLanguageContext.wrap(base)")
require(JAVA / "Preferences.java", 'KEY_APP_LANGUAGE = "angband.app_language"')
require(JAVA / "PreferencesActivity.java", "Preferences.KEY_APP_LANGUAGE.equals(key)")
require(JAVA / "PreferencesActivity.java", "recreate();")
require(JAVA / "NativeWrapper.java", "AppLanguageContext.usesTraditionalChinese(state.context)")
require(JAVA / "HelpActivity.java", "AppLanguageContext.usesTraditionalChinese(this)")
require(JAVA / "AppLanguageContext.java", "base.createConfigurationContext(config)")
require(JAVA / "AppLanguageSelection.java", 'FOLLOW_SYSTEM = "system"')
require(JAVA / "AngbandActivity.java", "R.id.btn_language")
require(JAVA / "AngbandActivity.java", "putString(Preferences.KEY_APP_LANGUAGE")
require(RES / "layout/splash.xml", "@string/splash_language")
require(ROOT / "scripts/test-terminal-unicode.sh", "AppLanguageSelectionTest")

namespace = "{http://schemas.android.com/apk/res/android}"
preferences = ET.parse(RES / "xml/preferences.xml").getroot()
language = next((p for p in preferences.iter()
                 if p.get(namespace + "key") == "angband.app_language"), None)
assert language is not None, "App language setting not found in preferences"
assert language.get(namespace + "defaultValue") == "system"
assert language.get(namespace + "entryValues") == "@array/app_language_values"
checks += 3

def resources(path):
    root = ET.parse(path).getroot()
    return {e.get("name"): e for e in root if e.get("name")}

english = resources(RES / "values/strings.xml")
chinese = resources(RES / "values-zh-rTW/strings.xml")
for name in ("splash_language", "app_language_title", "app_language_restart_hint",
             "app_language_restart_summary", "app_language_labels"):
    assert name in english and name in chinese, f"missing localized resource: {name}"
    checks += 1
assert [n.text for n in english["app_language_values"]] == [
    "system", "en", "zh-TW"], "Unstable language choice IDs"
assert len(english["app_language_labels"]) == len(chinese["app_language_labels"]) == 3
checks += 2

print(f"PASS: {checks} app language wiring checks")
