#!/usr/bin/env python3
"""Static checks for the first Traditional Chinese (Taiwan) Android UI localization.

Run from any directory:
    python3 scripts/check_zh_tw_resources.py

No Android SDK, network connection or third-party packages are required.
This does not replace an Android build or runtime CJK-rendering test.
"""
from pathlib import Path
import re
import sys
import xml.etree.ElementTree as ET


ROOT = Path(__file__).resolve().parents[1]
RES = ROOT / "app" / "src" / "main" / "res"
DEFAULT = RES / "values" / "strings.xml"
TRANSLATED = RES / "values-zh-rTW" / "strings.xml"
UI_XML = [
    RES / "xml" / "preferences.xml",
    RES / "layout" / "splash.xml",
    RES / "layout" / "profileadd.xml",
    RES / "layout" / "storage.xml",
    RES / "menu" / "profile.xml",
]
ANDROID = "{http://schemas.android.com/apk/res/android}"
RESOURCE_REF = re.compile(r"^@string/([A-Za-z0-9_]+)$")
JAVA_RESOURCE_REF = re.compile(r"\\bR\\.string\\.([A-Za-z0-9_]+)\\b")
FORMAT_ARGUMENT = re.compile(r"%(?:[0-9]+\\$)?[a-zA-Z]")
JAVA_UI = [
    ROOT / "app" / "src" / "main" / "java" / "org" / "rephial" / "xyangband" / file
    for file in (
        "GameActivity.java", "StorageActivity.java", "InputDialog.java",
        "HelpActivity.java", "AngbandActivity.java", "ProfilesActivity.java",
        "ProfileAddActivity.java",
    )
]

# These are used as persisted identifiers or default values, not just display labels.
# Do not localize these until the app uses separate stable entryValues.
PERSISTED = {
    "gamePlugins", "gamePluginNames", "orientations", "virtKeyActions",
    "graphicsModesKeys", "subWindowValues", "extraRibbonRows",
    "tileMultValues", "DPadColors", "FastKeysPopupPosition",
    "storageLocations", "keyboardPositionKey", "def_dpad_color1",
    "def_dpad_color2",
}

# Literal keyboard keycaps are kept as-is; they are not English UI copy.
SYMBOL_CAPS = (
    "Ampersand ", "Asterisk ", "At ", "Back Quote ", "Back Slash ",
    "Colon ", "Comma ", "Dollar ", "Double Quote ", "Equals ",
    "Exclamation ", "Forward Slash ", "Greater Than ", "Left Brace ",
    "Left Curly ", "Left Paren ", "Less Than ", "Minus ", "Percent ",
    "Period ", "Pipe ", "Plus ", "Pound ", "Question ", "Right Brace ",
    "Right Curly ", "Right Paren ", "Semicolon ", "Single Quote ",
    "Tilde ", "Underscore ",
)


def resources(path):
    root = ET.parse(path).getroot()
    entries = {}
    for child in root:
        if child.tag not in {"string", "string-array"}:
            continue
        key = child.get("name")
        if not key or key in entries:
            raise ValueError(f"{path}: duplicate or missing resource name: {key}")
        entries[key] = child
    return entries


def main():
    errors = []
    try:
        english = resources(DEFAULT)
        chinese = resources(TRANSLATED)
    except (ET.ParseError, ValueError, OSError) as exc:
        print(f"FAIL: {exc}")
        return 1

    for key, item in chinese.items():
        default = english.get(key)
        if default is None:
            errors.append(f"Missing English fallback resource: {key}")
            continue
        if default.tag != item.tag:
            errors.append(f"Resource type differs: {key}")
        if key in PERSISTED:
            errors.append(f"Persisted identifier must not be translated: {key}")
        if item.tag == "string-array" and len(default.findall("item")) != len(item.findall("item")):
            errors.append(f"Array item count differs: {key}")
        if item.tag == "string" and default.tag == "string":
            source_args = FORMAT_ARGUMENT.findall(default.text or "")
            localized_args = FORMAT_ARGUMENT.findall(item.text or "")
            if source_args != localized_args:
                errors.append(
                    f"Format placeholders differ: {key}: "
                    f"{source_args} != {localized_args}"
                )

    referenced = set()
    preference_untranslated = []
    for path in UI_XML:
        try:
            root = ET.parse(path).getroot()
        except (ET.ParseError, OSError) as exc:
            errors.append(f"Invalid UI XML: {path}: {exc}")
            continue
        for elem in root.iter():
            for name, value in elem.attrib.items():
                if name not in {
                    ANDROID + "title", ANDROID + "summary",
                    ANDROID + "text", ANDROID + "hint",
                }:
                    continue
                match = RESOURCE_REF.fullmatch(value)
                if match:
                    referenced.add(match.group(1))
                elif path.name == "preferences.xml" and name in {
                    ANDROID + "title", ANDROID + "summary",
                }:
                    if not re.fullmatch(r"[A-Z0-9]", value) and not value.startswith(SYMBOL_CAPS):
                        preference_untranslated.append(value)

    for path in JAVA_UI:
        try:
            source = path.read_text(encoding="utf-8")
        except OSError as exc:
            errors.append(f"Cannot read Java source: {path}: {exc}")
            continue
        referenced.update(JAVA_RESOURCE_REF.findall(source))

    for name in sorted(referenced):
        if name not in english:
            errors.append(f"Unresolved string resource: {name}")

    # Confirm translated help assets are present and referenced images still exist.
    assets = ROOT / "app" / "src" / "main" / "assets"
    help_zh = assets / "help_zh_tw.html"
    help_java = JAVA_UI[3]
    if not (assets / "help.html").is_file() or not help_zh.is_file():
        errors.append("Localized help and English fallback must both exist")
    else:
        guide = help_zh.read_text(encoding="utf-8")
        for image in re.findall(r'<img[^>]+src="([^"]+)"', guide):
            if not (assets / image).is_file():
                errors.append(f"Missing localized-help image: {image}")
        if 'help_zh_tw.html' not in help_java.read_text(encoding="utf-8"):
            errors.append("HelpActivity does not reference translated help")
    for label in preference_untranslated:
        errors.append(f"Unextracted preference label: {label}")

    if errors:
        print("FAIL: resource check")
        for error in errors:
            print("  - " + error)
        return 1

    print(
        "PASS: XML resources, Java/XML references, translation placeholders, "
        "stable preference values and help assets"
    )
    print(
        f"  English resources: {len(english)} | zh-TW resources: {len(chinese)} "
        f"| referenced UI strings: {len(referenced)}"
    )
    return 0


if __name__ == "__main__":
    sys.exit(main())
