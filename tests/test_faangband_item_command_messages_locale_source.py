#!/usr/bin/env python3
"""Guard visible FAangband item-command messages and English game fallback.

Only display labels and message templates may change: command keys, filters,
object IDs, inscription contents, mana checks and save data stay canonical.
"""
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[1]
FA = ROOT / "app/src/main/cpp/faangband"
SOURCE = (FA / "cmd-obj.c").read_text(encoding="utf-8")
UI = (ROOT / "app/src/main/cpp/angband/zh-tw-ui.h").read_text(
    encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")
NATIVE = (ROOT / "tests/native/test_fa_item_command_translation.c").read_text(
    encoding="utf-8")
RUNNER = (ROOT / "scripts/test-fa-item-command-translation.sh").read_text(
    encoding="utf-8")

assert '#include "../angband/zh-tw-ui.h"' in SOURCE

# Every literal passed to an item/spell selector must resolve to an existing
# display-only dictionary entry. Other command arguments remain unchanged.
SELECTORS = (
    "Uninscribe which item?",
    "You have nothing you can uninscribe.",
    "Inscribe which item?",
    "You have nothing to inscribe.",
    "Inscribe with what? ",
    "Take off or unwield which item?",
    "You have nothing to take off or unwield.",
    "Wear or wield which item?",
    "You have nothing to wear or wield.",
    "Replace which ring? ",
    "Drop which item?",
    "You have nothing to drop.",
    "Read which scroll? ",
    "You have no scrolls to read.",
    "Use which staff? ",
    "You have no staves to use.",
    "Aim which wand? ",
    "You have no wands to aim.",
    "Zap which rod? ",
    "You have no rods to zap.",
    "Activate which item? ",
    "You have no items to activate.",
    "Eat which food? ",
    "You have no food to eat.",
    "Quaff which potion? ",
    "You have no potions from which to quaff.",
    "Use which item? ",
    "You have no items to use.",
    "Refuel with which fuel source? ",
    "You have nothing you can refuel with.",
    "There are no spells you can cast.",
    "That book has no spells that you can cast.",
    "You cannot learn any new spells from the books you have.",
    "That book has no spells that you can learn.",
    "Study which book? ",
    "Attempt it anyway? ",
)
SIMPLE = (
    "Inscription removed.",
    "Hmmm, it seems to be stuck.",
    "That staff has no charges.",
    "That wand has no charges.",
    "That rod is still charging.",
    "That item is still charging.",
    "Equip the item to use it.",
    "The item cannot be used at the moment",
    "You fuel your lamp.",
    "Your lamp is full.",
    "You are not wielding a light.",
    "Your light cannot be refilled.",
    "You activate it.",
)
def string_literal(text):
    return '"' + text.replace('\\', '\\\\').replace('"', '\\"') + '"'

for english in SELECTORS + SIMPLE:
    literal = string_literal(english)
    assert literal + "," in UI, "No Chinese mapping: " + english
    # Keep literal source keys, permit the same English error in two workflows.
    pattern = (r"angdroid_game_ui_text\\(\\s*" +
               re.escape(literal) + r",\\s*angdroid_zh_tw_enabled\\(\\)\\s*\\)")
    assert re.search(pattern, SOURCE), "Untranslated callsite: " + english

# The English format path must still exist, and the Chinese path must use
# its own literal format string (not a dynamically translated printf format).
for english, chinese in (
    ("You failed to %s properly.", "你沒能正確使用法杖。"),
    ("The %s has no charges left.", "這根%s已沒有使用次數。"),
    ("Inscribing %s.", "正在為 %s 刻字。"),
    ("You cannot remove the %s you are %s.", "你無法卸下已裝備的 %s。"),
    ("Really take off %s? ", "確定卸下 %s 嗎？ "),
    ("%s %s (%c).", "你卸下了 %s（%c）。"),
    ("You see %s.", "你看到 %s。"),
    ("You have %s (1st %c).", "你有 %s（第一組：%c）。"),
    ("You have %s (%c).", "你有 %s（%c）。"),
    ("You do not have enough mana to %s this %s.", "你的法力不足，無法施放這個法術。"),
    ("You cannot learn any %ss in that book.", "這本書沒有可以學習的法術。"),
):
    assert string_literal(english) in SOURCE, english
    assert string_literal(chinese) in SOURCE, chinese

assert "You failed to %s properly." in SOURCE
for text in ("你沒能正確使用魔棒。", "你沒能正確使用魔杖。",
             "你沒能正確使用法杖。", "你沒能成功啟動這件物品。"):
    assert string_literal(text) in SOURCE
assert "msg(\"The %s has no charges left.\", what);" in SOURCE
assert "obj->known->activation = obj->activation;" in SOURCE
assert "obj->note = quark_add(str);" in SOURCE
assert "check_for_inscrip(equip_obj, \"!t\")" in SOURCE
assert "gear_to_label(player, equip_obj)" in SOURCE
assert "inven_drop(obj, amt);" in SOURCE
assert "lamp->timeout = z_info->fuel_lamp;" in SOURCE
assert "player->upkeep->energy_use = z_info->move_energy;" in SOURCE
assert '/* Verb */ "cast"' in SOURCE and '/* Verb */ "study"' in SOURCE
assert 'cmd_get_target(cmd, "target", &dir)' in SOURCE
assert "spell_cast(spell_index, dir, cmd)" in SOURCE
assert "spell_learn(spell_index)" in SOURCE
assert "angdroid_game_ui_text(\"Refuel with which fuel source? \"" in SOURCE
assert "Refuel with with fuel source" not in SOURCE

for source in (""Wear or wield which item?"", ""Inscription removed."",
               ""Attempt it anyway? "", ""You fuel your lamp.""):
    assert source in NATIVE, source
assert "-std=c99 -Wall -Wextra -Werror -pedantic" in RUNNER
assert "test-fa-item-command-translation.sh" in VERIFY
assert "test_faangband_item_command_messages_locale_source.py" in VERIFY
print("PASS: FA item action prompts, feedback, English fallback and command IDs")
