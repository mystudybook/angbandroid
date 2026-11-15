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
assert "test_faangband_help_zh_tw_source.py" in VERIFY
print("PASS: FA help viewer prompts, UTF-8 highlighting and menu links")
