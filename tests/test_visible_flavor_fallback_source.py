#!/usr/bin/env python3
"""Screen-only appearance fallback and unbroken generated scroll titles."""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SRC = (ROOT / "app/src/main/cpp/angband/obj-desc.c").read_text(encoding="utf-8")
TEST_RUN = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")
EXPORT = (ROOT / "app/src/main/cpp/angband/save-charoutput.c").read_text(encoding="utf-8")
RNG = (ROOT / "app/src/main/cpp/angband/obj-util.c").read_text(encoding="utf-8")

name = SRC.split("static size_t obj_desc_name(", 1)[1].split(
    "/* Append extra names of various kinds */", 1)[0]
start = name.index("/* The Chinese noun can still be shown")
finish = name.index("if (localized_scroll) {", start)
fallback = name[start:finish]
assert "(mode & ODESC_ZH_TW) && angdroid_zh_tw_enabled()" in fallback
assert "show_flavor && obj->kind->flavor && !obj->artifact" in fallback
assert "!obj->ego && obj->tval != TV_SCROLL" in fallback
assert "localized_flavor : obj->kind->flavor->text" in fallback
assert "if (aware && obj->kind->name)" in fallback
assert "if (!visible_effect) visible_effect = obj->kind->name;" in fallback
assert "if (appearance && *appearance && (!aware || visible_effect))" in fallback
assert "tval_find_name(obj->tval), appearance," in fallback

scroll = name.split("if (localized_scroll) {", 1)[1].split(
    "} else if (localized_kind) {", 1)[0]
assert 'angdroid_zh_tw_known_item_name(complete_scroll,' in scroll
assert '"scroll", scroll_display,' in scroll
assert 'prefix, number) ||' in scroll
assert 'angdroid_zh_tw_item_append_whole(buf, max, &end,' in scroll
assert 'angdroid_zh_tw_item_append_utf8(buf, max, &end, "卷軸");' in scroll
assert 'flavored_display))' in scroll
assert 'angdroid_zh_tw_identified_flavor_noun(' in scroll
assert 'angdroid_zh_tw_item_append_utf8(buf, max, &end, scroll_display)' not in scroll
assert 'angdroid_zh_tw_item_append_utf8(buf, max, &end, flavored_display)' not in scroll
assert "tests/native/test_visible_flavor_fallback_locale.c" in TEST_RUN
assert "tests/test_visible_flavor_fallback_source.py" in VERIFY
assert "ODESC_ZH_TW" not in EXPORT
assert "ODESC_ZH_TW" not in RNG
print("PASS: unidentified appearance preserved, known effect gated, scroll title atomic")
