#!/usr/bin/env python3
"""FA knowledge objects/runes/features/traps: translated visible text only.

Guard original identity, awareness/flavor, lore, and mobile column bounds.
The native dictionary tests remain under scripts/test-native-utf8.sh.
"""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp"
KNOW = (CPP / "faangband/ui-knowledge.c").read_text(encoding="utf-8")
UI = (CPP / "angband/zh-tw-ui.h").read_text(encoding="utf-8")
HELPER = (CPP / "faangband/obj-desc-item-zh-tw.h").read_text(
    encoding="utf-8")
RUNE = (CPP / "angband/zh-tw-rune-lore.h").read_text(encoding="utf-8")
TERRAIN = (CPP / "angband/zh-tw-terrain-trap.h").read_text(encoding="utf-8")
NATIVE = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")

def part(first, last):
    assert first in KNOW, first
    section = KNOW.split(first, 1)[1]
    assert last in section, last
    return section.split(last, 1)[0]

def require(section, *terms):
    for term in terms:
        assert term in section, term

obj = part("static void display_object(", "static void desc_obj_fake(")
obj_lore = part("static void desc_obj_fake(", "static int o_cmp_tval(")
obj_hint = part("static const char *o_xtra_prompt(", "static void o_xtra_act(")
obj_edit = part("static void o_xtra_act(", "void textui_browse_object_knowledge(")
rune_row = part("static void display_rune(", "static const char *rune_var_name(")
rune_lore = part("static void rune_lore(", "static const char *rune_xtra_prompt(")
rune_hint = part("static const char *rune_xtra_prompt(", "static void rune_xtra_act(")
rune_edit = part("static void rune_xtra_act(", "static void do_cmd_knowledge_runes(")
rune_menu = part("static void do_cmd_knowledge_runes(", "static void display_feature(")
feature_row = part("static void display_feature(", "static int f_cmp_fkind(")
feature_lore = part("static void feat_lore(", "static const char *feat_prompt(")
feature_hint = part("static const char *feat_prompt(", "static void f_xtra_act(")
feature_act = part("static void f_xtra_act(", "static void do_cmd_knowledge_features(")
trap_row = part("static void display_trap(", "static int trap_order(")
trap_lore = part("static void trap_lore(", "static const char *trap_prompt(")
trap_hint = part("static const char *trap_prompt(", "static void t_xtra_act(")
trap_act = part("static void t_xtra_act(", "static void do_cmd_knowledge_traps(")

require(KNOW,
    '#include "../angband/zh-tw-rune-lore.h"',
    '#include "../angband/zh-tw-terrain-trap.h"',
    '#include "obj-desc-item-zh-tw.h"',
    '#include "../angband/knowledge-column-text.h"')

# Name translations must never make an unaware/flavored object identified.
require(obj,
    'char o_name[160] = "";',
    'object_kind_name(o_name, sizeof(o_name), kind, OPT(player, cheat_xtra))',
    'if (kind->tried && !aware)',
    'angdroid_knowledge_tried_suffix(',
    'angdroid_zh_tw_enabled() && kind->aware',
    '(!kind->flavor || !OPT(player, show_flavors))',
    'angdroid_faangband_known_kind_ui_name(',
    'kind->name, tval_find_name(kind->tval), true',
    'if (localized) visible_name = localized;',
    'angdroid_knowledge_clip_label(clipped, sizeof(clipped),',
    'o_name, col, 45, Term->wid', # original source key documented below
    'angdroid_game_ui_text("Yes", angdroid_zh_tw_enabled())',
    'inscrip, 55, 75, Term->wid',
    'tile_height == 1 && Term && Term->wid > 76',
)
require(obj_lore,
    'object_info(obj, OINFO_FAKE)',
    'ODESC_PREFIX | ODESC_CAPITAL |',
    '(angdroid_zh_tw_enabled() ? ODESC_ZH_TW : 0)',
    'track_object_cancel(player->upkeep)',
)
require(obj_hint,
    'soft_kbd_flash("r{")',
    'if (ignore_tval(kind->tval)) soft_kbd_flash("s");',
    'soft_kbd_flash("}")',
    'return ignore_tval(kind->tval) ? with_insc : no_ignore_with_note;',
    'return ignore_tval(kind->tval) ? no_insc : no_ignore;',
)
require(obj_edit,
    'ignore_tval(k->tval)',
    'kind_ignore_when_aware(k)',
    'kind_ignore_when_unaware(k)',
    'add_autoinscription(oid, text, k->aware)',
    'angdroid_game_ui_text("Inscribe with: ",',
)

# Rune dictionaries and original IDs/autoinscriptions coexist safely.
require(rune_row,
    'rune_name(oid)',
    'angdroid_zh_tw_rune_title(',
    'label, col, 46, Term->wid',
    'inscrip, 47, Term->wid, Term->wid',
)
require(rune_lore,
    'angdroid_zh_tw_rune_title(',
    'angdroid_zh_tw_rune_description(',
    'rune_desc(oid), angdroid_zh_tw_enabled()',
)
require(rune_hint, 'soft_kbd_flash("r{")', 'soft_kbd_flash("}")')
require(rune_edit,
    'rune_set_note(oid, note_text)',
    'rune_autoinscribe(player, oid)',
    'angdroid_game_ui_text("Inscribe with: ",',
)
require(rune_menu,
    'player_knows_rune(player, i)',
    '"符文（%d 個尚未辨識）"',
    '"runes (%d unknown)"',
)
require(RUNE, 'angdroid_zh_tw_rune_title(', 'angdroid_zh_tw_rune_description(')
require(HELPER, 'angdroid_faangband_known_kind_ui_name(')

# Terrain and traps use canonical sort/group IDs; UTF-8 only in screen output.
for row, name_fn, symbol_field in (
    (feature_row, "angdroid_terrain_name", "feat_x_attr"),
    (trap_row, "angdroid_trap_name", "trap_x_attr"),
):
    require(row,
        name_fn + "(",
        "angdroid_knowledge_clip_label(clipped, sizeof(clipped),",
        "col, 64, Term->wid",
        "tile_height == 1 && Term && Term->wid > 65",
        "if (visual_col < Term->wid)",
        symbol_field + "[LIGHTING_DARK]",
        symbol_field + "[LIGHTING_LIT]",
        symbol_field + "[LIGHTING_TORCH]",
        symbol_field + "[LIGHTING_LOS]",
    )
    assert row.count("if (visual_col < Term->wid)") == 4

require(feature_lore, 'angdroid_terrain_name(', 'angdroid_terrain_lore(')
require(trap_lore, 'angdroid_trap_name(', 'angdroid_trap_lore(',
    'effect_describe(trap->effect, "This trap ", 0, false)')
require(feature_hint, 'soft_kbd_flash("tT");')
require(trap_hint, 'soft_kbd_flash("tT");',
    '''return ", 't/T' to cycle lighting";''')
require(feature_act, "ch.code == 't'", "ch.code == 'T'",
    'f_uik_lighting = LIGHTING_TORCH')
require(trap_act, "ch.code == 't'", "ch.code == 'T'",
    't_uik_lighting = LIGHTING_TORCH')
require(TERRAIN, 'angdroid_terrain_lore(', 'angdroid_trap_lore(')

# Do not pass CJK title bytes to a single-byte ASCII capitalization helper.
for lore in (rune_lore, feature_lore, trap_lore):
    require(lore,
        "if ((unsigned char)title[0] < 0x80u)",
        "my_strcap(title);")

for file in ('test_rune_lore_locale.c',
             'test_terrain_trap_locale.c',
             'test_faangband_trap_locale.c',
             'test_faangband_terrain_locale.c',
             'test_knowledge_column_text.c'):
    assert file in NATIVE
assert '"Inscribe with: "' in UI
assert 'test_faangband_knowledge_detail_locale_source.py' in VERIFY
print("PASS: FA object/rune/terrain/trap knowledge screens & narrow-screen safety")
