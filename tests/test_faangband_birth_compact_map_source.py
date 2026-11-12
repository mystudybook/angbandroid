#!/usr/bin/env python3
"""FAangband portrait birth details and bounded world-map browse contract.

Source-level checks keep display-only changes independent of selection and
birth command data. Real APK/device validation remains a separate step.
"""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SOURCE = (ROOT / "app/src/main/cpp/faangband/ui-birth.c").read_text(
    encoding="utf-8")
RUNNER = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")


def section(start, stop):
    assert start in SOURCE and stop in SOURCE, (start, stop)
    body = SOURCE.split(start, 1)[1]
    assert stop in body, stop
    return body.split(stop, 1)[0]


def require(body, *needles):
    for needle in needles:
        assert needle in body, needle


layout = section("static void fa_birth_configure_layout(void)",
                 "typedef void (*browse_f)")
require(layout,
    "if (Term->wid < 50)",
    "menu_width = MAX(1, MIN(36, Term->wid - RACE_COL - 1));",
    "rows = MAX(1, MIN(6, Term->hgt - TABLE_ROW - 7));",
    "race_region = (region){RACE_COL, TABLE_ROW, 17, MENU_ROWS};",
    "class_region = race_region;",
    "roller_region = race_region;")

compact = section("static int fa_birth_help_start(void)",
                  "static void race_help(")
require(compact,
    "return TABLE_ROW;",
    "TABLE_ROW + race_region.page_rows + 1",
    "static void fa_birth_help_clear(void)",
    "Term_erase(col, row, Term->wid - col);",
    "angdroid_menu_copy_columns(visible, sizeof(visible),",
    "screen_row < Term->hgt - 2",
    "angdroid_fa_history_wrap(label, (size_t)width,",
    "angdroid_fa_history_copy_line(original, sizeof(original),",
    "static void fa_birth_compact_details(",
    "race->r_adj[i] + (clazz ? clazz->c_adj[i] : 0)",
    "race->r_skills[i] + (clazz ? clazz->c_skills[i] : 0)",
    "skills[SKILL_TO_HIT_MELEE]",
    "skills[SKILL_DISARM_PHYS]",
    "skills[SKILL_DEVICE]",
    "race->r_mhp + (clazz ? clazz->c_mhp : 0)",
    "format(\"種族：%s\"",
    "format(\"職業：%s\"",
    "class_magic_realms(clazz, &count)",
    "of_has(clazz->flags, ability->index)",
    "pf_has(race->pflags, ability->index)",
    "mem_free(realm);",
    "fa_birth_detail_page_count = MAX(1,",
    "format(\"特性 %d/%d：按 [ 上頁、] 下頁\"",
    "fa_birth_help_line(&line, fa_birth_ability_visible_name(",
    "ability, zh));",
)
assert "cmdq_push(" not in compact
assert "CMD_CHOOSE_RACE" not in compact
assert "CMD_CHOOSE_CLASS" not in compact
race = section("static void race_help(", "static void class_help(")
clazz = section("static void class_help(",
                "static bool use_context_menu_birth(")
require(race,
    "if (fa_birth_compact_layout()) {",
    "fa_birth_compact_details(r, NULL);",
    "if (!r) return;",
    "text_out_hook = text_out_to_screen;")
require(clazz,
    "if (fa_birth_compact_layout()) {",
    "fa_birth_compact_details(r, c);",
    "if (!c) return;",
    "text_out_hook = text_out_to_screen;")

# Show at most three UTF-8-safe preview lines, leaving the current map's
# original description and ID untouched.
preview = section("static void map_menu_browser(",
                  "static void fa_birth_map_menu_resize(")
require(preview,
    "if (!description) description = \"\";",
    "angdroid_game_ui_text(d[oid].description,",
    "angdroid_fa_history_wrap(description, (size_t)width,",
    "angdroid_fa_history_copy_line(uncut, sizeof(uncut),",
    "max_lines = MIN((size_t)3, (size_t)(Term->hgt - row));",
    "show = MIN(lines, max_lines);",
    "Term_erase(col, row + (int)i, width);",
    "c_put_str(COLOUR_DEEP_L_BLUE, visible,",
)
assert "text_out_c(" not in preview
assert "Term_gotoxy(" not in preview

# An ESC or invalidated viewport should not select a previously highlighted
# map. Only the explicit EVENT_SELECT result can be submitted.
picker = section("static int map_menu_select(",
                 "static enum birth_stage get_map_command(")
require(picker,
    "ui_event result;",
    "int confirmed = -1;",
    "result = menu_select(m, 0, true);",
    "if (result.type == EVT_SELECT && current_map_index >= 0 &&",
    "current_map_index < num_maps)",
    "confirmed = current_map_index;",
    "current_map_index = confirmed;",
    "return confirmed;")
commit = section("static enum birth_stage get_map_command(",
                 "static struct menu race_menu")
require(commit,
    "if (map != -1)",
    "cmdq_push(CMD_CHOOSE_MAP);",
    'cmd_set_arg_choice(cmdq_peek(), "choice", map);')
assert "cmd_set_arg_choice(cmdq_peek(), \"choice\", m->cursor)" not in commit

assert "test_faangband_birth_compact_map_source.py" in RUNNER
print("PASS: FA compact race/class detail and bounded map selection source")
