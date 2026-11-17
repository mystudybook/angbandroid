#!/usr/bin/env python3
"""FAangband: player panel, spell UI and save picker are display-only zh-TW."""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
FA = ROOT / "app/src/main/cpp/faangband"
PLAYER = (FA / "ui-player.c").read_text(encoding="utf-8")
SPELL = (FA / "ui-spell.c").read_text(encoding="utf-8")
GAME = (FA / "ui-game.c").read_text(encoding="utf-8")
RUNNER = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")


def piece(src, start, end):
    assert start in src and end in src, (start, end)
    part = src.split(start, 1)[1]
    assert end in part, end
    return part.split(end, 1)[0]


def has(src, *words):
    for word in words:
        assert word in src, word


# Character-sheet labels and rating terms use a shared display dictionary.
# Use real Unicode columns for both left and right panels.
has(PLAYER,
    '#include "../angband/zh-tw-character-sheet.h"',
    '#include "../angband/zh-tw-class-titles.h"',
    '#include "../curses/unicode_display.h"',
    "char value[96];",
    "pl->label = angdroid_character_sheet_text(",
    "angdroid_game_ui_text(label, angdroid_zh_tw_enabled())")
panel = piece(PLAYER, "static void display_panel(",
              "static const char *show_title(")
has(panel,
    "angdroid_utf8_display_columns(pl->label)",
    "angdroid_utf8_copy_complete(label, sizeof(label), pl->label);",
    "angdroid_utf8_clip_columns(label,",
    "angdroid_utf8_clip_columns(value, (size_t)available);",
    "Term_putstr(value_x, row, -1, pl->attr, value);")
assert "strlen(pl->label)" not in panel
assert "strlen(pl->value)" not in panel
has(PLAYER,
    'angdroid_zh_tw_class_title(show_title(), angdroid_zh_tw_enabled())',
    'angdroid_character_sheet_text("Town", angdroid_zh_tw_enabled())',
    'angdroid_character_sheet_text("Normal", angdroid_zh_tw_enabled())',
    'angdroid_zh_tw_enabled() ? "無" : "None"',
    'angdroid_zh_tw_enabled() ? "召回點：" : "Recall pts: "')

# Character-dump sections remain in canonical English to avoid breaking
# exports and downstream tools, while on-screen success/error messages
# and c/f/h controls can be localized.
dump = piece(PLAYER, "void write_character_dump(", "bool dump_save(")
has(dump,
    "  [Character Equipment]",
    "  [Character Inventory]",
    "  [Character Quiver]",
    "  [Home Inventory]",
    "file_putf(fff, \"%c) %s\\n\"",
    "option_name(opt)")
change = piece(PLAYER, "void do_cmd_change_name(",
               "static void init_ui_player(")
has(change,
    'angdroid_character_sheet_text(',
    '"Character dump successful."',
    '"Character dump failed!"',
    'soft_kbd_flash("cfh")',
    "case 'c':", "case 'f':", "case 'h':",
    'angdroid_utf8_clip_columns(visible,',
    'Term_putstr(2, prompt_line, -1, COLOUR_WHITE, visible)')
has(PLAYER, 'if (get_character_name(namebuf, sizeof namebuf))')

# Savefile listing text changes, but filenames, ordering, default
# selection, panic save recovery, and serialized formats do not.
save_choice = piece(GAME, "static void select_savefile(", "void play_game(")
has(save_choice,
    'angdroid_game_ui_text("Use %s: %s", angdroid_zh_tw_enabled())',
    'angdroid_game_ui_text("Use %s", angdroid_zh_tw_enabled())',
    'angdroid_game_ui_text("New game", angdroid_zh_tw_enabled())',
    "names[count] = string_make(details->fnam);",
    "names[m->cursor]",
    "default_entry = 1;",
    "if (m->cursor == 0 && allow_new_game)",
    "m->flags |= MN_DBL_TAP",
    "fa_game_print_prompt(")
has(GAME,
    'angdroid_game_ui_text("A panic save exists.  Use it? "',
    'fa_game_print_prompt(angdroid_game_ui_text("Saving game..."',
    'angdroid_game_ui_text("Saving game... done."',
    'angdroid_game_ui_text("Saving game... failed!"',
    'angdroid_game_ui_text("Saving failed.  Try again? "',
    'angdroid_game_ui_text("death save failed!"',
    'savefile_save(savefile)',
    'prefs_save(path, option_dump, "Dump window settings")',
    'lore_save("lore.txt")')
has(GAME,
    'angdroid_menu_copy_columns(visible, sizeof(visible),',
    'static void fa_game_print_prompt(')

# Spell status and names come from source-exact shared translations.
# Original spell flags, integer indices, menu choices and damage math
# must be identical to the old implementation.
row = piece(SPELL, "static void spell_menu_display(",
            "static bool spell_menu_handler(")
has(row,
    'angdroid_zh_tw_spell_name(spell->name, angdroid_zh_tw_enabled())',
    '"(illegible)"',
    '" forgotten"',
    '" untried"',
    '" unknown"',
    '" difficult"',
    'PY_SPELL_FORGOTTEN', 'PY_SPELL_WORKED',
    'angdroid_menu_copy_columns(name, sizeof(name),',
    'angdroid_menu_copy_columns(visible, sizeof(visible),',
    'spell_chance(spell_index)',
    'get_spell_info(spell_index, help, sizeof(help))')
assert "utf8_clipto" not in row
description = piece(SPELL, "static void spell_menu_browser(",
                    "static const menu_iter spell_menu_iter")
has(description,
    'angdroid_zh_tw_spell_description(',
    'angdroid_projection_description(',
    'effect_avg_damage(e, shared_dice)',
    'PY_SPELL_FORGOTTEN',
    'angdroid_zh_tw_enabled() ? " 點傷害。" : " damage."')
has(SPELL,
    'static const char *fa_spell_menu_header(size_t max_columns)',
    'm->header = fa_spell_menu_header(width);',
    'angdroid_menu_copy_columns(visible, sizeof(visible), label,',
    'angdroid_zh_tw_magic_term(verb, true)',
    'angdroid_zh_tw_magic_term(noun, true)',
    'angdroid_game_ui_text("Browse which book? "',
    'angdroid_game_ui_text("You have no books that you can read."',
    'angdroid_game_ui_text("You cannot browse that."')
has(piece(SPELL, "static bool spell_menu_handler(",
          "static void spell_menu_browser("),
    'd->selected_spell = d->spells[oid];',
    "if (e->key.code == '?')")
has(piece(SPELL, "int textui_get_spell(", "return textui_get_spell_from_book("),
    '"選擇要%s的書本："',
    'get_item(&book, prompt, angdroid_game_ui_text(book_error')

assert "test_faangband_player_spell_save_locale_source.py" in RUNNER
print("PASS: FA character, spell and save UI keep gameplay and file IDs canonical")
