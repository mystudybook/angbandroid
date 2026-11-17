#!/usr/bin/env python3
"""FAangband spoiler menu display only: zh-TW labels and stable commands."""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SRC = (ROOT / "app/src/main/cpp/faangband/ui-spoil.c").read_text(
    encoding="utf-8")
MENU = (ROOT / "app/src/main/cpp/faangband/ui-menu.c").read_text(
    encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(
    encoding="utf-8")

def has(src, *items):
    for item in items:
        assert item in src, item

has(SRC,
    '#include "../angband/zh-tw-ui.h"',
    '"物品摘要 (obj-desc.spo)"',
    '"神器摘要 (artifact.spo)"',
    '"怪物摘要 (mon-desc.spo)"',
    '"完整怪物資訊 (mon-info.spo)"',
    '"產生遊戲資料檔" : "Create spoilers"',
    "fa_spoiler_set_labels(angdroid_zh_tw_enabled());",
    "if (Term) screen_load();",
    "spoil_menu->selections = all_letters_nohjkl;",
    "menu_layout(spoil_menu, &SCREEN_REGION);",
    "menu_select(spoil_menu, 0, false);",
    "cmdq_execute((player->is_dead) ? CTX_DEATH : CTX_GAME);",
    "event_signal(EVENT_MESSAGE_FLUSH);")
actions = SRC.split("static menu_action spoil_actions[] =", 1)[1].split(
    "static void fa_spoiler_set_labels(", 1)[0]
labels = SRC.split("static void fa_spoiler_set_labels(", 1)[1].split(
    "void do_cmd_spoilers(void)", 1)[0]
dispatch = SRC.split("static void spoiler_menu_act(", 1)[1].split(
    "static struct menu *spoil_menu", 1)[0]
entry = SRC.split("void do_cmd_spoilers(void)", 1)[1]
expected = ("CMD_SPOIL_OBJ", "CMD_SPOIL_ARTIFACT",
            "CMD_SPOIL_MON_BRIEF", "CMD_SPOIL_MON")
assert all(dispatch.count(x) == 1 for x in expected)
assert dispatch.count("cmdq_push(") == 4
assert actions.count("spoiler_menu_act") == 4
assert actions.count("{ 0, 0,") == 4
assert "menu_new_action(spoil_actions," in entry
assert entry.index("fa_spoiler_set_labels(") < entry.index("menu_select(")
assert "static struct menu *spoil_menu = NULL;" in SRC
assert "menu_setpriv(m, n, acts);" in MENU
assert "fa_menu_put_clipped(color, act->name, row, col, wid);" in MENU
assert "spoil_actions[0].name = zh_tw ?" in labels
assert "spoil_actions[3].name = zh_tw ?" in labels
assert "if (spoil_menu)" in labels
for filename in ("obj-desc.spo", "artifact.spo", "mon-desc.spo",
                 "mon-info.spo"):
    assert SRC.count(filename) >= 2, filename
assert "test_faangband_spoiler_menu_locale_source.py" in VERIFY
print("PASS: FAangband spoiler menu localized; action IDs and files unchanged")
