#!/usr/bin/env python3
"""FAangband in-game help UI and bundled reference file localization."""
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[1]
FA = ROOT / "variant_lib/faangband/help"
SRC = (ROOT / "app/src/main/cpp/faangband/ui-help.c").read_text(
    encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")

for token in (
    '#include "../angband/zh-tw-ui.h"',
    '#include "../angband/menu-text-fit.h"',
    '"說明文件「%s」"',
    '"無法開啟「%s」。"',
    '"[按字母選擇，ESC 離開]"',
    '"[按 ESC 離開]"',
    '"[空白鍵下一頁，ESC 離開]"',
    '"標示： "', '"搜尋： "', '"跳至行： "', '"開啟文件： "',
    'fa_help_prt(buf, i+2);',
    'fa_help_highlight(buf, (size_t)(str - lc_buf),',
    'angdroid_utf8_display_columns(prefix)',
    'angdroid_menu_copy_columns(visible, sizeof(visible),',
    'Term_get_size(&wid, &hgt);',
    'if (Term) screen_load();',
    'if (ch.code == \'/\')',
    'if (ch.code == \'%\')',
    'if (ch.code == \'!\')',
    'if (ch.code == \'#\')',
    'if (ch.code == \'&\')',
    'if (ch.code == \'?\')',
    'if (!show_file(hook[k], NULL, 0, mode))',
):
    assert token in SRC, token

for name, expected in {
    "index.txt": {"a": "commands.txt", "b": "symbols.txt"},
    "r_index.txt": {"a": "r_comm.txt", "b": "symbols.txt"},
}.items():
    text = (FA / name).read_text(encoding="utf-8")
    hooks = dict(re.findall(r'^\.\. menu:: \[([a-z])\] (\S+)$',
                            text, flags=re.MULTILINE))
    assert hooks == expected, (name, hooks)
    assert "說明瀏覽器操作" in text
    assert "faangband.readthedocs.io" in text
    assert "ESC 離開" in text
    assert text.count(".. menu:: ") == 2
# One command per line keeps Chinese labels readable on narrow terminals.
# Verify that neither keyset lost its original key count or special keys.
for filename, total, title in (
    ("commands.txt", 110, "原始按鍵配置：指令對照表"),
    ("r_comm.txt", 111, "Roguelike 按鍵配置：指令對照表"),
):
    doc = (FA / filename).read_text(encoding="utf-8")
    command_lines = [line for line in doc.splitlines()
                     if re.match(r"^  \S+\s{2,}", line)]
    keys = [line.strip().split()[0] for line in command_lines]
    assert len(keys) == total, (filename, len(keys))
    assert len(set(keys)) == total, filename
    assert title in doc, filename
    for special in ("^a", "^x", "?", "/", "!", "<", ">", "A", "a"):
        assert special in keys, (filename, special)
    assert "Ctrl" in doc and "查看" in doc
assert "TAB" in (FA / "r_comm.txt").read_text(encoding="utf-8")

# 98 symbols in four sections. Preserve map glyphs and punctuation.
SYMBOLS = (FA / "symbols.txt").read_text(encoding="utf-8")
assert "地圖符號速查表" in SYMBOLS
assert "預設 @ 代表玩家角色" in SYMBOLS
groups = (
    ("不阻擋視線的地形", 17),
    ("阻擋視線的地形", 8),
    ("物品", 19),
    ("怪物", 54),
)
for j, (heading, count) in enumerate(groups):
    section = SYMBOLS.split("\n" + heading + "\n", 1)[1]
    if j + 1 < len(groups):
        section = section.split("\n" + groups[j + 1][0] + "\n", 1)[0]
    entries = [line for line in section.splitlines()
               if re.match(r"^  \S\s{2,}", line)]
    assert len(entries) == count, (heading, len(entries))
MAKEFILE = (FA / "Makefile").read_text(encoding="utf-8")
for filename in ("commands.txt", "index.txt", "symbols.txt",
                 "r_comm.txt", "r_index.txt"):
    assert filename in MAKEFILE, filename

assert "test_faangband_help_zh_tw_source.py" in VERIFY
print("PASS: FA help viewer prompts, UTF-8 highlighting and menu links")
