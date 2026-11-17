#!/usr/bin/env python3
"""Source contract: FAangband visual/color, quality, ego, and option menus.

Translation remains display-only: original identifiers, user preferences,
stock kind knowledge, and menu selection callbacks must not be translated.
"""
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp"
SOURCE = (CPP / "faangband/ui-options.c").read_text(encoding="utf-8")
UI = (CPP / "angband/zh-tw-ui.h").read_text(encoding="utf-8")
RUN = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")

def part(start: str, end: str) -> str:
    assert start in SOURCE, start
    body = SOURCE.split(start, 1)[1]
    assert end in body, end
    return body.split(end, 1)[0]

def need(text: str, *strings: str) -> None:
    for string in strings:
        assert string in text, string

def canonical_names(array: str) -> list[str]:
    body = part("static const char *const " + array + "[] = {", "};")
    return re.findall(r'^\s*"([^"\n]+)",?\s*$', body, re.M)

# Cached dynamic menus must rebuild text from the same original English
# entries on every opening, while tags/callbacks stay fixed.
visual = canonical_names("visual_action_english")
color = canonical_names("color_action_english")
main = part("static const char *const option_action_english[] = {", "};")
assert len(visual) == 6
assert len(color) == 3
assert len(re.findall(r'^\s*(?:"[^"\n]+"|NULL),?\s*$', main, re.M)) == 20
assert len(set(visual)) == len(visual)
assert len(set(color)) == len(color)
for name in visual + color:
    assert ('{"' + name + '",') in UI, name

visual_menu = part("static void do_cmd_visuals(", "static void colors_pref_load(")
color_menu = part("static void do_cmd_colors(", "static bool askfor_aux_numbers(")
need(visual_menu,
    "N_ELEMENTS(visual_menu_items) == N_ELEMENTS(visual_action_english)",
    "visual_menu_items[i].name = angdroid_game_ui_text(",
    'visual_menu->title = title;',
    '"To edit visuals, use the knowledge menu"',
    'menu_select(visual_menu, 0, false)')
need(color_menu,
    "N_ELEMENTS(color_events) == N_ELEMENTS(color_action_english)",
    "color_events[i].name = angdroid_game_ui_text(",
    'color_menu->title = title;',
    'menu_select(color_menu, 0, false)')
need(SOURCE, 'angdroid_game_ui_text("Command: Modify colors"',
    '"顏色：%d  名稱：%s  索引：%c"',
    'angdroid_game_ui_text(name, true)',
    '"Command (n/N/k/K/r/R/g/G/b/B): "',
    '"Visual attr/char tables reset."')

# Translating callback titles must not change the canonical metadata of
# preference files, nor require a literal space in localized menu labels.
file_write = part("static void dump_pref_file(", "static void do_cmd_pref_file_hack(")
need(file_write, "const char *space = strchr(title, ' ');",
    'const char *item = space ? space + 1 : title;',
    'prefs_save(buf, dump, title)', 'msg(angdroid_zh_tw_enabled()')
for filename, title in (
    ("dump_monsters", "Save monster attr/chars"),
    ("dump_objects", "Save object attr/chars"),
    ("dump_features", "Save feature attr/chars"),
    ("dump_flavors", "Save flavor attr/chars"),
    ("dump_colors", "Dump colors"),
):
    need(SOURCE, f'dump_pref_file({filename}, "{title}", 15);')
assert 'dump_pref_file(dump_monsters, title, 15);' not in SOURCE

# UTF-8 terminal columns are the budget, never bytes or codepoints. The
# option keys remain their original stable configuration identifiers.
options = part("static void option_toggle_display(",
               "static bool option_toggle_handle(")
need(options,
    'angdroid_game_ui_text(option_desc(oid),',
    'angdroid_menu_copy_columns(clipped,',
    'available = MIN(width, Term->wid - col);',
    'desc_cols = MIN(45, MAX(0, available - 16));',
    'option_name(oid)',
    'options[oid]')
assert "utf8_clipto" not in options
need(part("static bool option_toggle_handle(",
          "static bool use_option_context_menu("),
     'option_set(option_name(oid), true)',
     'options_restore_custom(&player->opts, page)',
     'angdroid_game_ui_text("Successfully saved.  Press any key to continue."')
need(part("static bool use_option_context_menu(",
          "static const menu_iter option_toggle_iter"),
     '"Restore from saved defaults", angdroid_zh_tw_enabled()',
     '"Reset to factory defaults", angdroid_zh_tw_enabled()',
     'ACT_CTX_OPT_SAVE', 'ACT_CTX_OPT_RESTORE', 'ACT_CTX_OPT_RESET')

# Quality and ignore callbacks still use their original IDs, toggle state,
# and visible/unknown awareness; only names and positions are localized.
quality = part("static void quality_display(", "static void quality_subdisplay(")
need(quality,
    'quality_choices[oid].name, angdroid_zh_tw_enabled()',
    'quality_values[level].name, angdroid_zh_tw_enabled()',
    'angdroid_menu_copy_columns(clipped,',
    'available = MIN(width, Term->wid - col)',
    'ignore_level[oid]')
need(part("static void quality_subdisplay(", "static bool quality_action("),
     "quality_values[oid].name, angdroid_zh_tw_enabled()",
     'angdroid_menu_copy_columns(clipped,')
need(part("static bool quality_action(", "static void quality_menu("),
     'Term->wid < 9', 'Term->wid - area.width - 2',
     'ignore_level[oid] = menu.cursor;')
need(part("static void quality_menu(", "typedef struct"),
     '"Quality ignore menu", angdroid_zh_tw_enabled()')

ego = part("static void ego_display(", "static bool ego_action(")
need(ego,
    'angdroid_faangband_known_ego_ui_name(',
    'e_info[choice[oid].e_idx].name, true',
    'quality_choices[choice[oid].itype].name, true',
    'angdroid_menu_copy_columns(clipped_name,',
    'ego_item_name(buf, sizeof(buf), &choice[oid]);')
need(part("static void ego_menu(", "typedef struct"),
     'if (!ego->name || !ego->everseen)',
     'EGO_MENU_HELPTEXT_ZH : EGO_MENU_HELPTEXT',
     'text_out_wrap = MIN(79, MAX(1, Term->wid - 2));',
     'qsort(choice, max_num, sizeof(choice[0]), ego_comp_func)')
need(SOURCE,
    '"{light green}方向鍵{/}瀏覽清單',
    'fa_options_put_clipped(',
    'if (aware && kind->aware && angdroid_zh_tw_enabled())',
    'kind->ignore ^= IGNORE_IF_AWARE',
    'kind->ignore ^= IGNORE_IF_UNAWARE',
    'angdroid_game_ui_text(sval_dependent[line].desc,',
    'angdroid_game_ui_text(extra_item_options[line].name,')
need(UI, '"Reset visuals"', '"Quality ignore menu"',
     '"Save monster attr/chars"', '"Modify colors"')

assert 'test_faangband_options_visual_locale_source.py' in RUN
print("PASS: FA visual/color/options/ignore bilingual display-only contracts")
