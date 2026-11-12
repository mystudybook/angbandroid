#!/usr/bin/env python3
"""FAangband death/bones, restart/birth and item details source contract.

Translations affect only presentation. Menu tags, bones/savefile fields, birth
commands, item knowledge and actual gameplay remain canonical.
"""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
FA = ROOT / "app/src/main/cpp/faangband"
DEATH = (FA / "ui-death.c").read_text(encoding="utf-8")
BIRTH = (FA / "ui-birth.c").read_text(encoding="utf-8")
OBJECT = (FA / "ui-object.c").read_text(encoding="utf-8")
RUNNER = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")


def require(source, *strings):
    for string in strings:
        assert string in source, string


def section(source, first, last):
    assert first in source and last in source, (first, last)
    return source.split(first, 1)[1].split(last, 1)[0]


# Birth menu tags and callbacks stay English/canonical; language is selected
# each time the UI is presented.
require(DEATH,
    '#include "../angband/zh-tw-death.h"',
    'death_actions[label_index].name = angdroid_death_menu_label(',
    '(int)label_index, angdroid_zh_tw_enabled()',
    'ANGDROID_DEATH_DUMP_SUCCESS',
    'ANGDROID_DEATH_DUMP_FAILED',
    'ANGDROID_DEATH_NEW_GAME',
    'ANGDROID_DEATH_QUIT',
    'menu_new_action(death_actions,',
    'region area = { 51, 2, 0, N_ELEMENTS(death_actions) };')
menu = section(DEATH, "static menu_action death_actions[] =",
               "void death_screen(void)")
for key in ("'i'", "'m'", "'f'", "'v'", "'x'", "'h'", "'s'", "'n'", "'q'"):
    assert key in menu, key
assert menu.index('"Quit"') > menu.index('"New Game"')
require(DEATH,
    "if (e.key.code == KTRL('X')) break;",
    "if (e.key.code == KTRL('N')) play_again = true;",
    'area.col = MAX(0, Term->wid - 24);')

# Tombstone and victory artwork: center and clip by terminal cell width,
# not utf8_strlen() or glyph count, and display translated static prose only.
tomb = section(DEATH, "static void put_str_centred(",
               "static char *get_personalized_string(")
require(tomb,
    "angdroid_menu_copy_columns(visible, sizeof(visible),",
    "x = left + (width - columns) / 2;",
    'angdroid_zh_tw_class_title(',
    'angdroid_game_ui_text(player->class->name,',
    '"等級：%d"', '"經驗：%d"', '"金幣：%d"',
    '"命喪%s第%d層"', '"死因：%s"',
    'player->died_from')
assert "utf8_strlen(tmp)" not in tomb
require(DEATH,
    '"向偉大冠軍致敬！"',
    'int x = MAX(0, (wid - width) / 2);',
    'if (i >= hgt || x >= wid) break;')

# CJK personalized ghost messages cannot end in partial bytes. Bone filename,
# race/class numbers, flags and M/D/y/n decision characters are preserved.
ghost = section(DEATH, "static char *get_personalized_string(",
                "static void display_winner(void)")
require(ghost,
    'angdroid_utf8_copy_complete(cleaned, sizeof(cleaned), tmp);',
    "while (n > 0 && cleaned[n - 1] == ' ')",
    'memcmp(end, "。", 3)',
    'memcmp(end, "！", 3)',
    'memcmp(end, "？", 3)',
    'my_strcpy(tmp, cleaned, sizeof(tmp));',
    'static char tmp[80], info[80];',
    'soft_kbd_flash("${yes_no}")',
    'file_putf(fp, "%d:%s\\n", choice, string)',
    'file_putf(fp, "%d\\n", player->race->ridx)',
    'file_putf(fp, "%d\\n", player->class->cidx)',
    '"bone.%03d"',
    "answer.key.code == 'y'",
    "answer.key.code == 'm'",
    "answer.key.code == 'd'")
assert '%-79.79s' not in ghost
require(DEATH,
    "fa_death_print_prompt(angdroid_zh_tw_enabled() ?",
    '"最多 %d 位元組，內容會作為"')

# Inspecting inventory at death remains a read-only view; observed item
# identities, filenames and all original item choices remain unchanged.
info = section(DEATH, "static void death_info(",
               "static void death_messages(")
require(info,
    'ANGDROID_DEATH_MORE_INFORMATION', 'ANGDROID_DEATH_EQUIPMENT',
    'ANGDROID_DEATH_INVENTORY', 'ANGDROID_DEATH_QUIVER',
    'ANGDROID_DEATH_HOME_PAGE',
    "MIN(12, MAX(1, Term->hgt - 5))",
    "angdroid_menu_copy_columns(visible, sizeof(visible),",
    '(angdroid_zh_tw_enabled() ? ODESC_ZH_TW : 0)')
require(section(DEATH, "static void death_examine(",
                "static void death_history("),
    'ANGDROID_DEATH_EXAMINE', 'ANGDROID_DEATH_NO_ITEMS',
    'ODESC_CAPITAL', 'OINFO_NONE',
    '(angdroid_zh_tw_enabled() ? ODESC_ZH_TW : 0)')

# Birth hints use the shared dictionary and Unicode screen centering.
# Command IDs and literal Y/N/C/=, r/p, S, ESC behaviors cannot be localized.
require(BIRTH,
    '#include "../angband/zh-tw-ui.h"',
    'static void fa_birth_prompt_centered(',
    'angdroid_utf8_display_columns(message)',
    'angdroid_menu_copy_columns(visible, sizeof(visible), message,',
    'x = columns >= Term->wid ? 0 : (Term->wid - columns) / 2;',
    '#define BIRTH_MENU_HELPTEXT_ZH',
    'BIRTH_MENU_HELPTEXT_ZH : BIRTH_MENU_HELPTEXT',
    'angdroid_game_ui_text(roller_choices[i], angdroid_zh_tw_enabled())')
assert BIRTH.count('fa_birth_prompt_centered(prompt, Term->hgt - 1);') == 4
for cmd in ("CMD_BIRTH_RESET", "CMD_ACCEPT_CHARACTER",
            "CMD_NAME_CHOICE", "CMD_HISTORY_CHOICE"):
    require(BIRTH, cmd)
require(BIRTH,
    "ke.code == 'Y'", "ke.code == 'N'",
    "ke.code == 'S'", "ke.code == '='",
    "angdroid_game_ui_text(\"Total Cost: %2d/%2d\"",
    '"A savefile for that name exists.  Overwrite it? "',
    '"Accept character history? [y/n]"')

# Object descriptions must not be forced into Chinese when the language
# is English, and the quiver labels fit mobile terminal widths.
require(OBJECT,
    'ODESC_PREFIX | ODESC_FULL | (angdroid_zh_tw_enabled() ? ODESC_ZH_TW : 0)',
    'ODESC_NOEGO | ODESC_BASE | ODESC_PLURAL | (angdroid_zh_tw_enabled() ? ODESC_ZH_TW : 0)',
    'ODESC_CAPITAL | (angdroid_zh_tw_enabled() ? ODESC_ZH_TW : 0)',
    'angdroid_game_ui_text("(nothing)"',
    '"箭袋位置 %d："',
    '"箭袋：%d 支彈藥%s"',
    "if (Term && col + 3 < Term->wid)",
    "cmd_verb(cmd)",
    'verify_object(prompt_buf, obj, player)')
assert OBJECT.count('(angdroid_zh_tw_enabled() ? ODESC_ZH_TW : 0)') >= 5

assert "test_faangband_death_birth_locale_source.py" in RUNNER
print("PASS: FA death, birth and item localization preserves IDs and UTF-8")
