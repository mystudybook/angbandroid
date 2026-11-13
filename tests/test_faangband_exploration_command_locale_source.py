#!/usr/bin/env python3
"""FAangband player exploration messages use zh-TW UI labels, not game IDs."""
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[1]
SOURCE = (ROOT / "app/src/main/cpp/faangband/cmd-cave.c").read_text(
    encoding="utf-8")
UI = (ROOT / "app/src/main/cpp/angband/zh-tw-ui.h").read_text(
    encoding="utf-8")
NATIVE = (ROOT / "tests/native/test_fa_item_command_translation.c").read_text(
    encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")

EXISTING = (
    "Nothing happens!",
    "You enter a maze of up staircases.",
    "You enter a maze of down staircases.",
    "The dungeon does not appear to extend deeper",
    "You see nothing there.",
    "You see nothing there to open.",
    "You have picked the lock.",
    "You failed to pick the lock.",
    "There is a monster in the way!",
    "You see nothing there to close.",
    "You're standing in that doorway.",
    "The door appears to be broken.",
    "This seems to be permanent rock.",
    "You see nothing there to tunnel.",
    "You have found something!",
    "You see nothing there to disarm.",
    "You lock the door.",
    "You failed to lock the door.",
    "You spin around.",
    "You feel a door blocking your way.",
    "You feel a wall blocking your way.",
    "There is a door blocking your way.",
    "There is a wall blocking your way.",
    "There is a pile of rubble in the way!",
    "There is a wall in the way!",
    "You clear the web.",
    "You cannot explore while confused.",
    "Something is here.",
    "No known path to downstairs.",
    "No known path to upstairs.",
    "No apparent path for exploration.",
    "There is a scream and the door slams shut!",
    "Looks like a typical town.",
    "No target monster selected!",
    "This monster has no spells!",
    "The monster can not move.",
    "You hear a door burst open!",
    "The way is blocked.",
    "Valid commands: move, stand still, 'd'rop, 'm'agic, or 'r'elease.",
)
NEW = (
    ("You already live here!", "你已經住在這裡了！"),
    ("Your home will be here when you return.", "你回來時，家仍會在這裡。"),
    ("You can only move to another town!", "只能搬到另一座城鎮！"),
    ("This is a path to greater danger.", "這條路通往更危險的地方。"),
    ("This staircase leads down.", "這道樓梯通往下層。"),
    ("This level will never appear again.  Really leave?", "離開後就無法回到這一層。確定要離開嗎？"),
    ("This level will never appear again.  Really leave? ", "離開後就無法回到這一層。確定要離開嗎？ "),
    ("You trigger a magic portal.", "你啟動了一座魔法傳送門。"),
    ("You enter a winding path to less danger.", "你踏上了通往較安全地方的蜿蜒小徑。"),
    ("This is a path to less danger.", "這條路通往較安全的地方。"),
    ("This staircase leads up.", "這道樓梯通往上層。"),
    ("You enter a winding path to greater danger.", "你踏上了通往更危險地方的蜿蜒小徑。"),
    ("You can not see to set a trap.", "你看不清楚，無法設置陷阱。"),
    ("You are too confused.", "你過於混亂，無法設置陷阱。"),
    ("You must disarm an existing trap to free up your equipment.", "必須先拆除一個已設置的陷阱，才能騰出器材。"),
    ("You set a monster trap.", "你設置了一個怪物陷阱。"),
)

def literal(text):
    return '"' + text.replace("\\", "\\\\").replace('"', '\\"') + '"'

assert '#include "../angband/zh-tw-ui.h"' in SOURCE
for english in EXISTING + tuple(en for en, _ in NEW):
    raw = literal(english)
    assert raw + "," in UI, "Missing source dictionary: " + english
    pattern = r"angdroid_game_ui_text\(\s*" + re.escape(raw) + (
        r",\s*angdroid_zh_tw_enabled\(\)\s*\)")
    assert re.search(pattern, SOURCE), "Unlocalized visible callsite: " + english

for english, chinese in NEW:
    pair = "{" + literal(english) + ", " + literal(chinese) + "}"
    assert pair in UI, "Incorrect wilderness mapping: " + english
    assert pair in NATIVE, "No bilingual regression: " + english

# Both themed-level confirmation variants include distinct trailing spaces.
assert SOURCE.count("get_check(angdroid_game_ui_text(") >= 2
assert 'format("你真的要搬到 %s 嗎？", town)' in SOURCE
assert 'format("Do you really want to move to %s?", town)' in SOURCE
assert 'history_add(player, format("Moved house to %s.", town)' in SOURCE

# Formatted notices keep their exact English string and independent Chinese
# literal format: no variable-format msg()/msgt() to break NDK -Wformat-security.
for english, chinese in (
    ("You have disarmed your %s.", "你拆除了自己設置的%s。"),
    ("You have removed the %s.", "你移除了%s。"),
    ("You have disarmed the %s.", "你拆除了%s。"),
    ("You failed to disarm the %s.", "你沒能拆除%s。"),
    ("You set off the %s!", "你觸發了%s！"),
    ("You are too afraid to attack %s!", "你太害怕，不敢攻擊%s！"),
    ("%s drops %s.", "%s 掉落了 %s。"),
    ("%s slams against the door.", "%s 猛撞門扉。"),
    ("%s fiddles with the lock.", "%s 試圖撬開門鎖。"),
):
    assert literal(english) in SOURCE, english
    assert literal(chinese) in SOURCE, chinese

# Only text branches changed; changing towns, stairs, traps and pathfinding
# retain canonical state mutations and English save/history details.
for invariant in (
    "player->home = player->place;",
    "player_change_place(player, new_place);",
    "player->upkeep->create_stair = return_path(feat);",
    "square_set_door_lock(cave, grid, power);",
    "square_destroy_trap(cave, grid);",
    "square_remove_trap(cave, grid, trap, true)",
    "player->num_traps--;",
    "player_of_has(p, OF_AFRAID)",
    "equip_learn_flag(p, OF_AFRAID);",
    "hit_trap(grid, -1);",
):
    assert invariant in SOURCE, invariant

assert "test_faangband_exploration_command_locale_source.py" in VERIFY
assert "test-fa-item-command-translation.sh" in VERIFY
print("PASS: FAangband exploration zh-TW, English fallback and gameplay invariants")
