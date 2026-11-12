#!/usr/bin/env python3
"""FAangband specialty menus and shape lore share display-only zh-TW labels.

Source-level checks: C99/NDK compilation and Android acceptance remain separate.
"""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
FA = ROOT / "app/src/main/cpp/faangband"
UI = (FA / "ui-player-properties.c").read_text(encoding="utf-8")
MENU = (FA / "ui-menu.c").read_text(encoding="utf-8")
INPUT = (FA / "ui-input.c").read_text(encoding="utf-8")
NATIVE_DROID = (ROOT / "app/src/main/cpp/curses/droid.c").read_text(
    encoding="utf-8")
PROPS = (FA / "player-properties.c").read_text(encoding="utf-8")
LORE = (FA / "ui-knowledge.c").read_text(encoding="utf-8")
DUMP = (FA / "ui-player.c").read_text(encoding="utf-8")
RUNNER = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")
LAYOUT = (FA / "ui-ability-compact-layout.h").read_text(encoding="utf-8")
HISTORY = (FA / "ui-birth-history-utf8.h").read_text(encoding="utf-8")
C_TEST = (ROOT / "tests/native/test_faangband_ability_layout.c").read_text(encoding="utf-8")
RUN_SCRIPT = (ROOT / "scripts/test-fa-ability-layout.sh").read_text(encoding="utf-8")


def section(source, begin, end):
    assert begin in source and end in source, (begin, end)
    return source.split(begin, 1)[1].split(end, 1)[0]


def require(source, *pieces):
    for piece in pieces:
        assert piece in source, piece


require(UI,
    '#include "../angband/zh-tw-player-ability.h"',
    '#include "../angband/menu-text-fit.h"',
    '#include "ui-birth-abilities-zh-tw.h"',
    '#include "ui-birth-elements-zh-tw.h"',
    '"學習專精或檢視能力（l/v/Esc）？"',
    '"沒有可學習的專精能力。"',
    'mem_free(d->specialties);',
    'mem_free(d);')

view = section(UI, "static void view_ability_display(",
               "static void view_ability_menu_browser(")
require(view,
    "choices[oid].type, choices[oid].index, zh",
    'angdroid_fa_birth_ability_name(',
    '"特殊能力：%s"', '"職業能力：%s"', '"種族能力：%s"',
    '"Specialty Ability: %s"', '"Class: %s"', '"Racial: %s"',
    'streq(choices[oid].type, "element")',
    "angdroid_fa_birth_element_name(choices[oid].index)",
    "angdroid_fa_birth_element_resistance(choices[oid].value)",
    '"免疫：%s"', '"抗性：%s"', '"弱點：%s"',
    "angdroid_menu_copy_columns(visible, sizeof(visible), buf,",
    "c_put_str(cursor ? COLOUR_WHITE : color, visible, row, col);")

browser = section(UI, "static void view_ability_menu_browser(",
                  "void textui_view_ability_menu(")
require(browser, "angdroid_ability_description(choices[oid].desc,",
        "angdroid_zh_tw_enabled()")

learn = section(UI, "static void gain_spec_display(",
                "static bool gain_spec_action(")
require(learn, "lookup_ability(\"player\", idx, 0)",
        "if (!ability) return;",
        "angdroid_fa_birth_ability_name(ability->name,",
        "angdroid_menu_copy_columns(visible, sizeof(visible), name,",
        "c_put_str(attr, visible, row, col);")

# Generic menu_select() tries the row handler for an outside-list touch
# before falling through to menu_handle_mouse() / context_hook.
outside_touch = section(
    MENU,
    "/* Keep shopkeeper/header and outside context actions. */",
    "} else if (in.type == EVT_KBRD)")
require(outside_touch,
        "if (!no_act && menu_handle_action(menu, &in))",
        "menu_handle_mouse(menu, &in, &out);")
assert outside_touch.index("menu_handle_action(menu, &in)") < (
    outside_touch.index("menu_handle_mouse(menu, &in, &out);"))

action = section(UI, "static bool gain_spec_action(",
                 "static void gain_spec_menu_browser(")
require(action,
        "if (!e || !d) return false;",
        "if (fa_ability_handle_page(menu, e, oid)) return true;",
        "oid < 0 || oid >= d->spec_known",
        "!check_specialty_gain(d->specialties[oid])",
        "d->selected_spec = d->specialties[oid];",
        "return e->type != EVT_MOUSE;")

learn_browser = section(UI, "static void gain_spec_menu_browser(",
                        "bool textui_gain_spec_menu(")
require(learn_browser, "if (!ability) return;",
        "angdroid_ability_description(ability->desc,")

learn_menu = section(UI, "bool textui_gain_spec_menu(",
                     "void textui_interact_with_specialties(")
require(learn_menu, '"學習哪項專精（%c-%c，Esc 離開，剩餘 %d 次）？"',
        '"%s，確定學習？"', '"%s: learn this specialty? "',
        "menu_select(&menu, EVT_SELECT, true);",
        "d->selected_spec = 0;",
        "if (evt.type != EVT_SELECT) break;",
        "bool accepted = false;",
        "accepted = true;",
        "if (accepted)",
        "*pick = d->selected_spec;",
        "return accepted;",
        "clear_from(0);",
        "fa_ability_layout_menu(&menu);",
        "mem_free(d->specialties);")
assert "if (evt.type != EVT_ESCAPE)" not in learn_menu
# Modal key ownership: clear menu tags before get_check() appends Yes/No;
# after cancellation the list must refresh on the next menu_select().
assert "soft_kbd_replace(\"\");" in learn_menu
assert learn_menu.count('soft_kbd_replace("");') == 3
assert learn_menu.index('menu_refresh(&menu, false);') < learn_menu.index(
    'soft_kbd_replace("");') < learn_menu.index("if (get_check(format(")
assert "d->selected_spec = 0;\n        soft_kbd_replace(\"\");" in learn_menu
assert learn_menu.index('if (accepted)') < learn_menu.rindex(
    'soft_kbd_replace("");') < learn_menu.index("screen_load();")
view_menu = section(UI, "void textui_view_ability_menu(",
                    "struct spec_menu_data {")
assert view_menu.index("menu_select(&menu, 0, false);") < (
    view_menu.index('soft_kbd_replace("");')) < view_menu.index("screen_load();")
ability_outer = section(UI, "void textui_interact_with_specialties(",
                        "\n}")
assert ability_outer.strip().endswith('soft_kbd_replace("");')
assert learn_menu.index("if (accepted)") < learn_menu.index(
    "*pick = d->selected_spec;")
assert "pf_on(" not in UI and "pf_off(" not in UI
assert "cmdq_push(" not in UI

require(LORE, '#include "ui-birth-elements-zh-tw.h"',
        '#include "../angband/zh-tw-player-ability.h"',
        "bool zh = angdroid_zh_tw_enabled();",
        "zh ? angdroid_fa_birth_element_name(index) : NULL;",
        "angdroid_shape_term(projections[index].name, zh);")
shape = section(LORE, "static void shape_lore_append_resistances(",
                "static void shape_lore_append_protection_flags(")
assert shape.count("fa_shape_element_visible_name(i)") == 3
require(shape, "s->el_info[i].res_level == RES_LEVEL_MAX",
        "s->el_info[i].res_level < RES_LEVEL_BASE",
        "s->el_info[i].res_level > RES_LEVEL_BASE")

misc = section(LORE, "static void shape_lore_append_misc_flags(",
               "static void shape_lore_append_change_effects(")
require(misc, "angdroid_ability_description(",
        "angdroid_shape_misc_desc(ability->desc,",
        "if (localized == ability->desc)",
        "pf_has(s->pflags, ability->index)")

require(UI,
    "return oid >= 0 && (size_t)oid < strlen(all_letters_nohjkl) ?",
    "all_letters_nohjkl[last_shortcut]",
    "last_shortcut = MIN(num_abilities,")
require(PROPS,
    "list = mem_alloc((size_t)capacity * sizeof(*list));",
    "lookup_ability(\"player\", i, 0)",
    "list[count++].group = PLAYER_FLAG_SPECIAL;",
    "list[count++].group = PLAYER_FLAG_CLASS;",
    "list[count++].group = PLAYER_FLAG_RACE;",
    "view_ability_menu(list, count);",
    "mem_free(list);")
for token in (
    "if (!class || !ability || !ability->type || ability->index < 0)",
    "if (!race || !ability || !ability->type || ability->index < 0)",
    "ability->index < PF_MAX &&",
    "ability->index < OF_MAX &&",
    "ability->index < ELEM_MAX &&",
    "race->el_info[ability->index].res_level == ability->value;",
):
    assert token in PROPS, token
assert "ability_list[32]" not in PROPS
assert "if (!ability || !ability->name) return;" in PROPS
assert "if (pick <= PF_NONE || pick >= PF_MAX ||" in PROPS
assert PROPS.index('if (!ability || !ability->name) return;') < PROPS.index(
    "pf_on(player->specialties, pick);")


# The selectable list and text description have independent bounded space;
# portrait view and new specialty dialogs share the same resize callback.
require(UI,
    '#include "ui-ability-compact-layout.h"',
    '#include "ui-birth-history-utf8.h"',
    "static void fa_ability_layout_menu(struct menu *m)",
    "static void fa_ability_resize_menu(struct menu *m)",
    "static void fa_ability_draw_description(",
    "menu_layout(m, &bounds);",
    "fa_ability_detail_oid = -1;",
    "fa_ability_detail_pages = total_pages;",
    "fa_ability_draw_description(",
    "angdroid_fa_history_wrap(text, (size_t)width,",
    "angdroid_fa_history_copy_line(raw, sizeof(raw),",
    "angdroid_menu_copy_columns(visible, sizeof(visible), raw,",
    "Term_erase(0, row, Term->wid);",
    "fa_ability_handle_page(",
    "static bool fa_ability_view_action(",
    "if (event && event->type == EVT_SELECT) return true;",
    "fa_ability_page_touch",
    "event->mouse.y != Term->hgt - 1",
    "event->mouse.x >= MIN(Term->wid, 80)",
    "key.key.code = event->mouse.x < MIN(Term->wid, 80) / 2 ? '[' : ']';")
assert UI.count('menu.cmd_keys = "[]";') == 2
assert UI.count("fa_ability_layout_menu(&menu);") >= 2
assert "70, -99" not in UI and "text_out_wrap = 60;" not in UI
assert "fa_ability_resize_menu };" in UI
assert "gain_spec_action, fa_ability_resize_menu };" in UI
assert "angdroid_fa_ability_page_clamp(" in UI
assert "loc.page_rows" not in UI  # stale variable from the old layout
assert "starts[256]" not in UI
assert "capacity = angdroid_fa_ability_line_capacity(strlen(text));" in UI
assert "#include <limits.h>" in UI
assert "total_pages = pages > (size_t)INT_MAX ? INT_MAX : (int)pages;" in UI
assert "starts = mem_alloc(capacity * sizeof(*starts));" in UI
assert "lengths = mem_alloc(capacity * sizeof(*lengths));" in UI
assert "mem_free(lengths);" in UI and "mem_free(starts);" in UI
assert UI.count('menu.cmd_keys = "[]";') == 2
assert "if (specialty <= PF_NONE || specialty >= PF_MAX) return false;" in UI
assert "ability = lookup_ability(\"player\", specialty, 0);" in UI
assert "if (!ability || !ability->name) return false;" in UI
assert 'if (!name) name = zh ? "未知能力" : "Unknown ability";' in UI
assert "if (ability && ability->name && check_specialty_gain(idx))" in UI
assert "if (get_check(format(" in UI

assert "void fa_ability_layout_menu" in UI
assert "int angdroid_fa_ability_plan(" in LAYOUT
assert "int angdroid_fa_ability_page_clamp(" in LAYOUT
assert "angdroid_fa_history_wrap(" in HISTORY
assert "test_terminal_geometry(void)" in C_TEST
assert "test_description_paging(void)" in C_TEST
assert "test_dynamic_line_capacity(void)" in C_TEST
HISTORY_TEST = (ROOT / "tests/test_faangband_birth_history_utf8.c").read_text(
    encoding="utf-8")
assert 'count != 321 || starts[320] != 640' in HISTORY_TEST
assert "angdroid_fa_ability_line_capacity(strlen(long_text))" in HISTORY_TEST
assert "angdroid_fa_ability_line_capacity(size_t bytes)" in LAYOUT
assert '-std=c99 -Wall -Wextra -Werror -pedantic' in RUN_SCRIPT
assert 'test-fa-ability-layout.sh' in RUNNER
assert 'test-fa-ability-action.sh' in RUNNER
assert 'test-fa-ability-hints.sh' in RUNNER
assert 'soft_kbd_flash("${yes_no}");' in INPUT
assert 'soft_kbd_replace(buf);' in MENU
assert 'void soft_kbd_replace(const char *keys)' in NATIVE_DROID
assert 'Term_control_keys("${clear}");' in NATIVE_DROID
HINT_GEN = (ROOT / "tests/native/generate_fa_ability_hint_host.py").read_text(
    encoding="utf-8")
assert 'extract(name) for name in names' in HINT_GEN
for fn in ("soft_kbd_flash", "soft_kbd_replace", "soft_kbd_clear",
           "soft_kbd_flush", "strdeldup"):
    assert fn in HINT_GEN
assert '"${yes_no}"' in HINT_GEN
assert '"${clear}"' in HINT_GEN
HINT_RUN = (ROOT / "scripts/test-fa-ability-hints.sh").read_text(
    encoding="utf-8")
assert "generate_fa_ability_hint_host.py" in HINT_RUN
assert "-std=c99 -Wall -Wextra -Werror -pedantic" in HINT_RUN
ACTION_GEN = (ROOT / "tests/native/generate_fa_ability_action_host.py").read_text(
    encoding="utf-8")
assert "matched.group(0)" in ACTION_GEN
assert "EVT_MOUSE" in ACTION_GEN and "EVT_SELECT" in ACTION_GEN
ACTION_RUN = (ROOT / "scripts/test-fa-ability-action.sh").read_text(
    encoding="utf-8")
assert "generate_fa_ability_action_host.py" in ACTION_RUN
assert "-std=c99 -Wall -Wextra -Werror -pedantic" in ACTION_RUN

# Export and original player ability data are deliberately not localized.
dump = section(DUMP, "void write_character_dump(", "bool dump_save(")
require(dump, '"  [Character Equipment]\\n\\n"',
        'lookup_ability("player", i, 0)->name',
        'option_name(opt)')
# The common FAangband confirmation input must not truncate UTF-8 in
# raw bytes, including long or malformed labels, or hide the Yes/No keys.
confirm = section(INPUT, "static bool textui_get_check(",
                  "/* TODO: refactor get_check() in terms of get_char() */")
require(INPUT, '#include "ui-confirm-prompt-utf8.h"')
require(confirm,
        "angdroid_fa_confirm_prompt_line(buf, sizeof(buf), prompt, width);",
        "if (ke.type == EVT_RESIZE)",
        "soft_kbd_replace(\"\");",
        'soft_kbd_flash("${yes_no}");',
        "if (width < 3)",
        "ke = inkey_ex();",
        "return (ke.type == EVT_KBRD || ke.type == EVT_BUTTON) &&")
assert 'strnfmt(buf, 78, "%.70s[y/n] " ' not in confirm\nassert "inkey_m();" not in confirm
PROMPT = (FA / "ui-confirm-prompt-utf8.h").read_text(encoding="utf-8")
require(PROMPT, "../angband/menu-text-fit.h",
        "angdroid_fa_confirm_prompt_line(",
        "if (width > capacity - 1) width = capacity - 1;",
        "angdroid_menu_copy_columns(dest, capacity - suffix_bytes,",
        'memcpy(dest + used, suffix, suffix_bytes + 1);')
NATIVE_CONFIRM = (ROOT / "tests/native/test_faangband_confirm_prompt_utf8.c").read_text(
    encoding="utf-8")
assert "test_all_widths(void)" in NATIVE_CONFIRM
assert "test_long_chinese_and_byte_boundaries(void)" in NATIVE_CONFIRM
CONFIRM_GENERATOR = (
    ROOT / "tests/native/generate_fa_confirm_check_host.py").read_text(
        encoding="utf-8")
assert "match.group(0)" in CONFIRM_GENERATOR
assert 'textui_get_check\\(const char' in CONFIRM_GENERATOR
assert "EVT_RESIZE" in CONFIRM_GENERATOR
CONFIRM_RUNNER = (ROOT / "scripts/test-fa-confirm-check.sh").read_text(
    encoding="utf-8")
assert "generate_fa_confirm_check_host.py" in CONFIRM_RUNNER
assert "test_faangband_confirm_prompt_utf8.c" in CONFIRM_RUNNER
assert "-std=c99 -Wall -Wextra -Werror -pedantic" in CONFIRM_RUNNER
assert "test-fa-confirm-check.sh" in RUNNER

assert "test_faangband_ability_browser_locale_source.py" in RUNNER
print("PASS: FA specialty browsers, typed labels and shape knowledge source guard")
