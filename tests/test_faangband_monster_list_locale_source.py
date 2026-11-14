#!/usr/bin/env python3
"""FAangband monster list geometry, UTF-8 and recall display-only regression."""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp"
def get(path):
    return (ROOT / path).read_text(encoding="utf-8")

LIST = get("app/src/main/cpp/faangband/ui-mon-list.c")
NAME = get("app/src/main/cpp/faangband/mon-desc.c")
LORE = get("app/src/main/cpp/faangband/ui-mon-lore.c")
DICT = get("app/src/main/cpp/angband/zh-tw-ui.h")
GEOMETRY = get("app/src/main/cpp/angband/monster-list-compact-layout.h")
COLUMNS = get("app/src/main/cpp/curses/unicode_display.h")
NATIVE = get("tests/native/test_faangband_monster_list_locale.c")
RUNNER = get("scripts/test-fa-monster-list-locale.sh")
VERIFY = get("scripts/verify-zh-tw.sh")

section = LIST.split("static void monster_list_format_section(", 1)[1].split(
    "static bool monster_list_format_special(", 1)[0]
interactive = LIST.split("void monster_list_show_interactive(", 1)[1].split(
    "void monster_list_force_subwindow_update(", 1)[0]
subwindow = LIST.split("void monster_list_show_subwindow(", 1)[1].split(
    "void monster_list_show_interactive(", 1)[0]
special = LIST.split("static bool monster_list_format_special(", 1)[1].split(
    "static void monster_list_format_textblock(", 1)[0]

for filename in [
    "monster-list-compact-layout.h", "zh-tw-monster-list-ui.h",
    "zh-tw-list-location.h"
]:
    assert '#include "../angband/' + filename + '"' in LIST

for token in [
    "angdroid_monster_row_fit(max_width,",
    "angdroid_monster_sleep_fits(full_width, tag_columns)",
    "angdroid_list_relative_location(location, sizeof(location)",
    "angdroid_utf8_display_columns(location)",
    "angdroid_utf8_display_columns(asleep)",
    "angdroid_utf8_display_columns(line_buffer)",
    "angdroid_utf8_clip_columns(line_buffer, name_width)",
    "angdroid_utf8_printf_field_bytes(",
    "if (!budget.show_location) location[0] = '\\0';",
    '"%s有 %d 隻%s怪物%s\\n"',
    '"%s沒有怪物。\\n"',
    "（睡眠中、中立）",
    "（%d 隻睡眠、%d 隻中立）",
    "（中立）",
    "……另有 %d 隻怪物。",
]:
    assert token in section, "Missing list output/guard: " + token

assert "full_width = max_width - 2" not in section
assert "full_width - utf8_strlen(asleep)" not in section
assert "utf8_clipto(" not in section
assert "strnfmt(location," not in section
assert "char asleep[72]" in section
assert 'angdroid_monster_list_hallucination_text(zh_tw)' in special
assert "angdroid_monster_list_hallucination_columns(zh_tw)" in special
assert 'if (height < 1 || width < 7)' in subwindow
assert 'if (height < 7 || width < 12)' in interactive
assert "angdroid_monster_list_popup_fit(width, height," in interactive
assert "textblock_free(tb);" in interactive
assert "monster_list_free(list);" in interactive
assert "safe_width = popup.width;" in interactive
assert "safe_height = popup.height;" in interactive
assert "r.col = -safe_width;" in interactive
assert "safe_width = MIN(width - 40" not in interactive
assert 'soft_kbd_flash("x")' in interactive
assert "angdroid_monster_list_sort_hint(" in interactive
assert "angdroid_utf8_copy_complete(buf, sizeof(buf)," in interactive
assert "while (ch.code == 'x')" in interactive

# Full UTF-8 scalar copy when the user has a narrow monster list.
assert '#include "../curses/unicode_display.h"' in NAME
assert "if (angdroid_zh_tw_enabled()) {" in NAME
assert 'strnfmt(formatted, sizeof(formatted), "[U] %s（%s）"' in NAME
assert 'strnfmt(formatted, sizeof(formatted), "%3d 隻%s %s"' in NAME
assert "angdroid_utf8_copy_complete(buf, buflen, formatted);" in NAME
assert 'strnfmt(buf, buflen, "[U] %s, the %s"' in NAME
assert "plural_aux(buf, buflen);" in NAME

# Lore screen is a view; known flags, spoilers and unique colour are untouched.
assert '#include "../angband/zh-tw-ui.h"' in LORE
assert 'textblock_append(tb, "這隻");' in LORE
assert 'zh_tw ? "%s（%s）" : "%s, the %s"' in LORE
assert "OPT(player, purple_uniques)" in LORE
for phrase in [
    "You know everything about this monster.",
    "You feel an intense desire to kill this monster...  "
]:
    assert phrase in DICT
    assert "angdroid_game_ui_text(" in LORE
assert "lore_append_attack(tb, race, lore, known_flags);" in LORE
assert "lore_append_spells(tb, race, lore, known_flags);" in LORE

assert "screen_width < 12 || screen_height < 7" in GEOMETRY
assert "content - location_columns >= 4" in GEOMETRY
assert "angdroid_utf8_clip_columns" in COLUMNS
assert "angdroid_utf8_printf_field_bytes" in COLUMNS
assert "-std=c99 -Wall -Wextra -Werror -pedantic" in RUNNER
assert "test_faangband_monster_list_locale.c" in RUNNER
assert "test-fa-monster-list-locale.sh" in VERIFY
assert "test_faangband_monster_list_locale_source.py" in VERIFY
assert "angdroid_monster_list_popup_fit" in NATIVE
assert "angdroid_utf8_clip_columns" in NATIVE
print("PASS: FA monster list compact widths and bilingual recall display")
