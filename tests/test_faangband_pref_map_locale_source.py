#!/usr/bin/env python3
"""FAangband map and preference-file messages: translate UI, preserve data."""
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
FA=ROOT/"app/src/main/cpp/faangband"
PREF=(FA/"ui-prefs.c").read_text(encoding="utf-8")
MAP=(FA/"ui-map.c").read_text(encoding="utf-8")
RUN=(ROOT/"scripts/verify-zh-tw.sh").read_text(encoding="utf-8")

for s in (
    '#include "../angband/zh-tw-ui.h"',
    '"無法建立檔案 %s" : "Failed to create file %s"',
    '"設定檔 %s 第 %d 行第 %d 欄解析錯誤：%s：%s"',
    '"Parse error in %s line %d column %d: %s: %s"',
    '"無法開啟「%s」。" : "Cannot open \'%s\'."',
    's.line, s.col, s.msg, parser_error_str[s.error]',
    'if (!quiet)',
    'e = PARSE_ERROR_INTERNAL;',
    'parser_parse(p, line)',
    'event_signal(EVENT_MESSAGE_FLUSH);',
):
    assert s in PREF, s
world=MAP.split("/* Now print the info */",1)[1].split(
    "/* End of function */",1)[0] if "/* End of function */" in MAP else MAP
assert '"第 %d 層" :' in world
assert '"Level %d", world->levels[place[i]].depth' in world
assert "print_map_name(place[i], row, col, false);" in world
assert "world->levels[place[i]].east" in world
assert "test_faangband_pref_map_locale_source.py" in RUN
print("PASS: FA pref diagnostic labels and world-map level names localized")
