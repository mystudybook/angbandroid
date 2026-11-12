#!/usr/bin/env python3
"""FAangband world map, race/class creation, item inscription display contract.

Translations must not change map/character selection indexes, birth commands,
object inscriptions, or the native keyboard and touch menu action IDs.
"""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
FA = ROOT / "app/src/main/cpp/faangband"
BIRTH = (FA / "ui-birth.c").read_text(encoding="utf-8")
OBJECT = (FA / "ui-object.c").read_text(encoding="utf-8")
UI = (ROOT / "app/src/main/cpp/angband/zh-tw-ui.h").read_text(
    encoding="utf-8"
)
RUNNER = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")


def assert_has(source, *terms):
    for term in terms:
        assert term in source, term


def section(source, start, end):
    assert start in source and end in source, (start, end)
    return source.split(start, 1)[1].split(end, 1)[0]


# World artwork and map choice IDs are still driven by native map records.
# Only the intro, visible label, help and menu region are different.
map_intro = section(BIRTH, "#define MAP_TEXT", "struct map_menu_data")
assert_has(map_intro,
    "#define MAP_TEXT_ZH",
    "歡迎來到第一紀元 Angband！",
    "FAangband 有多種世界地圖配置",
    "angdroid_zh_tw_enabled() ? MAP_TEXT_ZH : MAP_TEXT",
    "text_out_wrap = saved_wrap;",
    "text_out_indent = saved_indent;",
)
map_rows = section(BIRTH, "static void map_menu_display(",
                   "static const menu_iter map_menu_iter")
assert_has(map_rows,
    "angdroid_game_ui_text(d[oid].name,",
    "angdroid_menu_copy_columns(visible, sizeof(visible), name,",
    "current_map_index = oid;",
    "current_map_index = -1;",
    "show_file(\"mapmode.txt\", NULL, 0, 0);",
    "text_out_wrap = saved_wrap;",
    "text_out_indent = saved_indent;",
    "text_out_pad = saved_pad;",
)
map_choose = section(BIRTH, "static struct menu *map_menu_new(",
                     "static struct menu race_menu")
assert_has(map_choose,
    "d[i].name = map->name;",
    "d[i].description = map->help;",
    'd[i].name = "Quit";',
    "map == num_maps - 1",
    "cmdq_push(CMD_CHOOSE_MAP);",
    'cmd_set_arg_choice(cmdq_peek(), "choice",',
    "loc.width = MAX(1, MIN(70, Term->wid - loc.col - 1));",
    "loc.page_rows = MIN(num_maps + 1,",
)

# Portrait layout focuses one menu and one details pane; landscape preserves
# the original three columns. Race, class, and roller indices remain the same.
layout = section(BIRTH, "static region race_region =",
                 "typedef void (*browse_f)")
assert_has(layout,
    "static bool fa_birth_compact_layout(void)",
    "Term->wid < 76",
    "fa_birth_configure_layout(void)",
    "race_region = (region){RACE_COL, TABLE_ROW, 17, MENU_ROWS};",
    "class_region = race_region;",
    "roller_region = race_region;",
    "static int fa_birth_detail_col(int normal_col)",
)
assert_has(BIRTH,
    "menu_layout(&race_menu, &race_region);",
    "menu_layout(&class_menu, &class_region);",
    "menu_layout(&roller_menu, &roller_region);",
    "if (!fa_birth_compact_layout())",
    "menu_refresh(&race_menu, false);",
    "menu_refresh(&class_menu, false);",
    "fa_birth_detail_col(RACE_AUX_COL)",
    "fa_birth_detail_col(CLASS_AUX_COL)",
    "text_out_wrap = saved_wrap;",
)

# The CJK birth bonus and skill vocabulary has the same format argument list,
# and the original game values (not localized names) drive all calculations.
stats = section(BIRTH, "static void skill_help(",
                "static bool use_context_menu_birth(")
assert_has(stats,
    "近戰／射擊／投擲：%+d/%+d/%+d",
    "解除陷阱：%+3d/%+3d",
    "豁免：%+3d",
    "挖掘：%+3d",
    "夜視：%d 英尺",
    "難度：等級 %d",
    "故鄉：%s",
    "可學習%s魔法",
    "angdroid_game_ui_text(ability->name,",
    "angdroid_game_ui_text(realm->name,",
    "skills[SKILL_TO_HIT_MELEE]",
    "skills[SKILL_SAVE]",
    "r->difficulty",
    "c->magic.total_spells",
)

# The point-buy costs previously lived at x=74, outside portrait screens.
# The positions now adapt to width without changing bought/sold values.
costs = section(BIRTH, "#define COSTS_ROW", "static enum birth_stage point_based_command(")
assert_has(costs,
    "static int fa_birth_costs_col(void)",
    "static int fa_birth_total_col(void)",
    "if (wid >= 80) return 42 + 32;",
    "if (wid >= 80) return 42 + 19;",
    "fa_birth_put_clipped(",
    "angdroid_menu_copy_columns(visible, sizeof(visible), message,",
    "spent[i]",
    "inc[i] <= remaining",
    "sum += spent[i]",
    "MIN(Term->wid - 1, COSTS_COL + 4)",
)
assert_has(BIRTH,
    "cmdq_push(CMD_SELL_STAT);",
    "cmdq_push(CMD_BUY_STAT);",
    "cmdq_push(CMD_RESET_STATS);",
    "cmdq_push(CMD_ACCEPT_CHARACTER);",
    "cmd_set_arg_choice(cmdq_peek(), \"choice\", stat);",
)

# Both keyboard and contextual touch menu options keep their literal tags.
for label in (
    "Show birth options", "Select one at random",
    "Finish with random choices", "Quit", "Help",
    "Reroll", "Retrieve previous", "Accept", "Sell", "Buy", "Reset"
):
    assert ('{"' + label + '",') in UI, label
    assert ('angdroid_game_ui_text("' + label + '"') in BIRTH, label
assert_has(BIRTH,
    "menu_dynamic_add_label(m, angdroid_game_ui_text(",
    "soft_kbd_flash(\"rp?\\x18\")",
    "current_menu->cursor = randint0(current_menu->count)",
)

# Shared exact-verb matching only changes the visible `Really ...`
# confirmation. The !* / !key rule, number of checks, and IDs persist.
verify = section(OBJECT, "bool get_item_allow(", "static bool get_tag(")
assert_has(OBJECT, '#include "../angband/zh-tw-inscription-confirm.h"')
assert_has(verify,
    'char verify_inscrip[] = "!*";',
    'verify_inscrip[1] = ch;',
    'check_for_inscrip(obj, verify_inscrip)',
    'check_for_inscrip(obj, "!*")',
    'cmd_verb(cmd)',
    "angdroid_inscription_confirm_prefix(prompt_buf,",
    "sizeof(prompt_buf), verb, angdroid_zh_tw_enabled())",
    'strnfmt(prompt_buf, sizeof(prompt_buf), "Really %s", verb)',
    "while (n--)",
    "verify_object(prompt_buf, obj, player)",
)
assert "test_faangband_birth_map_touch_locale_source.py" in RUNNER
print("PASS: FA birth map, portrait layout and inscription confirmation locale")
