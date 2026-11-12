#!/usr/bin/env python3
"""FAangband item examination/ignore UI must use existing zh-TW display labels."""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp"
code = (CPP / "faangband/ui-object.c").read_text(encoding="utf-8")
lookup = (CPP / "angband/zh-tw-ui.h").read_text(encoding="utf-8")
quality = (CPP / "angband/zh-tw-ignore-quality-action.h").read_text(encoding="utf-8")

for key in (
    "Examine which item?", "You have nothing to examine.",
    "This item only", "Unignore this item",
    "Ignore which item? ", "You have nothing to ignore.",
    "(Enter to select, ESC) Ignore:", "Quality ignore menu",
):
    assert ('"' + key + '"') in lookup, key
    assert ('"' + key + '"') in code, key

assert '#include "../angband/zh-tw-ignore-quality-action.h"' in code
assert "angdroid_ignore_quality_action_text(" in code
assert "angdroid_zh_tw_enabled()" in code
assert '#include "../angband/ignore-named-action-locale.h"' in code
assert '#include "obj-desc-item-zh-tw.h"' in code
assert "angdroid_ignore_named_action_text(" in code
assert "angdroid_faangband_known_ego_ui_name(" in code
assert "ODESC_NOEGO | ODESC_BASE | ODESC_PLURAL | ODESC_ZH_TW" in code
assert "soft_kbd_save_linger(&parent_keys);" in code
assert 'soft_kbd_replace("");' in code
assert "soft_kbd_restore_linger(&parent_keys, selected < 0);" in code
assert "menu_dynamic_add(m, out_val, IGNORE_THIS_QUALITY)" in code
assert code.count("char header_buf[256];") >= 3
assert "menu_dynamic_longest_entry(m) + 3 + 2" in code
assert "Term->wid - 2" in code
assert "All %s %s" in quality  # Unmapped labels retain English.
print("PASS: FAangband item examine and ignore UI is localized without changing IDs")
