#!/usr/bin/env python3
"""FAangband knowledge/Ego labels and scoped ignore popups stay display-only."""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp"
KNOW = (CPP / "faangband/ui-knowledge.c").read_text(encoding="utf-8")
OBJ = (CPP / "faangband/ui-object.c").read_text(encoding="utf-8")
HELPER = (CPP / "angband/ignore-named-action-locale.h").read_text(
    encoding="utf-8")
UI = (CPP / "angband/zh-tw-ui.h").read_text(encoding="utf-8")
RUN = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")

for include in (
    '#include "../angband/zh-tw-ui.h"',
    '#include "../angband/zh-tw-knowledge-groups.h"',
    '#include "../angband/knowledge-column-text.h"',
    '#include "obj-desc-item-zh-tw.h"',
):
    assert include in KNOW, include

assert "angdroid_knowledge_group_text(" in KNOW
assert "angdroid_utf8_display_columns(g_names[i])" in KNOW
assert 'prt(format("知識圖鑑：%s",' in KNOW
assert 'angdroid_game_ui_text(title, true)' in KNOW
assert '"分類" : "Group"' in KNOW
assert '"名稱" : "Name"' in KNOW

display = KNOW.split("static void display_ego_item(", 1)[1].split(
    "static void desc_ego_fake(", 1)[0]
recall = KNOW.split("static void desc_ego_fake(", 1)[1].split(
    "static int e_cmp_tval(", 1)[0]
for section in (display, recall):
    assert "angdroid_faangband_known_ego_ui_name(" in section
    assert "ego->name" in section
    assert "angdroid_zh_tw_enabled()" in section
assert "angdroid_knowledge_clip_label(" in display
assert "Term->wid" in display
assert "angdroid_knowledge_group_text(" in recall
assert "object_info_ego(ego)" in recall
assert "format(\"%s %s\", group, label)" in recall

ignore = OBJ.split("void textui_cmd_ignore_menu(", 1)[1].split(
    "void textui_cmd_ignore(", 1)[0]
assert "ODESC_NOEGO | ODESC_BASE | ODESC_PLURAL | ODESC_ZH_TW" in ignore
assert "angdroid_faangband_known_ego_ui_name(" in ignore
assert "angdroid_ignore_named_action_text(" in ignore
assert "ego_display = localized ? localized :" in ignore
assert "soft_kbd_save_linger(&parent_keys);" in ignore
assert "soft_kbd_replace(\"\");" in ignore
assert "soft_kbd_restore_linger(&parent_keys, selected < 0);" in ignore
assert ignore.index("soft_kbd_save_linger(&parent_keys);") < ignore.index(
    "selected = menu_dynamic_select(m);")
assert ignore.index("soft_kbd_replace(\"\");") > ignore.index(
    "selected = menu_dynamic_select(m);")
assert ignore.index("soft_kbd_restore_linger(&parent_keys, selected < 0);"
    ) > ignore.index("soft_kbd_replace(\"\");")
for token in ("IGNORE_THIS_FLAVOR", "UNIGNORE_THIS_FLAVOR",
              "IGNORE_THIS_EGO", "UNIGNORE_THIS_EGO", "IGNORE_THIS_QUALITY"):
    assert token in ignore
assert "ignore_level[ignore_type] = ignore_value;" in ignore
assert "obj->known->notice |= OBJ_NOTICE_IGNORE;" in ignore

for key in ("Ignore items of this type", "Unignore items of this type"):
    assert f'\"{key}\"' in UI
    assert f'\"{key}\"' in ignore
assert 'snprintf(out, capacity, "%s%s", prefix, name)' in HELPER
assert 'if (n < 0 || (size_t)n >= capacity)' in HELPER
assert "test_ignore_named_action_locale.c" in RUN
assert "test_faangband_knowledge_ego_locale_source.py" in VERIFY
ANG_OBJ = (CPP / "angband/ui-object.c").read_text(encoding="utf-8")
for engine, text in (("Angband", ANG_OBJ), ("FAangband", OBJ)):
    modal = text.split("void textui_cmd_ignore_menu(", 1)[1].split(
        "void textui_cmd_ignore(", 1)[0]
    assert "soft_kbd_linger_snapshot parent_keys;" in modal, engine
    assert "soft_kbd_save_linger(&parent_keys);" in modal, engine
    assert 'soft_kbd_replace("");' in modal, engine
    assert "soft_kbd_restore_linger(&parent_keys, selected < 0);" in modal, engine
    assert modal.index("soft_kbd_save_linger(&parent_keys);") < modal.index(
        "selected = menu_dynamic_select(m);"), engine
    assert modal.index("soft_kbd_restore_linger(&parent_keys, selected < 0);"
        ) > modal.index("selected = menu_dynamic_select(m);"), engine

print("PASS: FA knowledge/Ego names and modal ignore actions localized safely")
