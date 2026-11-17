#!/usr/bin/env python3
"""FAangband command picker coverage: all names are display-only translated."""
from pathlib import Path
import re
ROOT=Path(__file__).resolve().parents[1]
CPP=ROOT/"app/src/main/cpp"
GAME=(CPP/"faangband/ui-game.c").read_text(encoding="utf-8")
CONTEXT=(CPP/"faangband/ui-context.c").read_text(encoding="utf-8")
CORE=(CPP/"angband/zh-tw-ui.h").read_text(encoding="utf-8")
RUNNER=(ROOT/"scripts/verify-zh-tw.sh").read_text(encoding="utf-8")
CMD=GAME.split("struct cmd_info cmd_item[]")[1].split("struct command_list cmds_all[]")[0]
names=[x.replace(r'\"', '"') for x in re.findall(
    r'^\s*\{\s*"((?:[^"\\]|\\.)*)"\s*,\s*\{', CMD, re.M)]
CAT=GAME.split("struct command_list cmds_all[]")[1].split("/*** Exported functions")[0]
groups=re.findall(r'^\s*\{\s*"([^"]+)"', CAT, re.M)
def labels(text):
    matches=re.findall(r'\{\s*"((?:[^"\\]|\\.)+)",\s*"((?:[^"\\]|\\.)+)"\s*\}',text)
    return {a.replace(r'\"','"'): b for a,b in matches}
local=CONTEXT.split("static const char *fa_context_ui_text(")[1].split(
    "/* The prompt row owns only")[0]
translated=labels(CORE)
translated.update(labels(local))
assert len(names)==len(set(names))==124
assert len(groups)==16
assert all(key in translated for key in names+groups),[
    key for key in names+groups if key not in translated]
for s in ('"變身"','"搬家"','"查看遊戲時間"','"除錯：傳送"',
          '"不儲存直接離開"',
          "fa_context_ui_text(cmd_list[i].desc)",
          "fa_context_ui_text(commands[oid].desc)",
          "fa_context_ui_text(cmds_all[oid].name)",
          "cmd_list_lookup_by_name(list->list[menu.cursor].nested_name)",
          "Term_keypress(cmd_list[selected-1].key[mode], 0)"):
    assert s in CONTEXT,s
assert "return angdroid_game_ui_text(english, true);" in local
assert "if (!angdroid_zh_tw_enabled() || !english) return english;" in local
assert "test_faangband_command_picker_locale_source.py" in RUNNER
print("PASS: 124 commands, 16 categories; original identifiers and keys preserved")
