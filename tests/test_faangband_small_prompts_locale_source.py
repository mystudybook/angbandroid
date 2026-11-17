#!/usr/bin/env python3
"""FAangband color picker/keymap/file-pref prompts only: screen-only locale."""
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
FA=ROOT/"app/src/main/cpp/faangband"
K=(FA/"ui-knowledge.c").read_text(encoding="utf-8")
O=(FA/"ui-options.c").read_text(encoding="utf-8")
RUN=(ROOT/"scripts/verify-zh-tw.sh").read_text(encoding="utf-8")

glyph=K.split("static void display_glyphs(",1)[1].split(
    "static bool glyph_command(",1)[0]
assert 'angdroid_zh_tw_enabled() ? "選擇顏色：" : "Choose colour:"' in glyph
assert "Term_locate(&x, &y);" in glyph
assert "for (i = 0; i < MAX_COLORS; i++) big_pad(" in glyph
assert "Term_gotoxy(x + a, y);" in glyph

keymap=O.split('c_prt(color, angdroid_zh_tw_enabled() ?',1)[1].split(
    "kp = inkey();",1)[0]
assert 'format("按鍵映射最多 %d 鍵。", KEYMAP_ACTION_MAX)' in keymap
assert 'format("(Maximum keymap length is %d keys.)",' in keymap
assert "KEYMAP_ACTION_MAX" in keymap
pref=O.split("static void do_cmd_pref_file_hack(",1)[1].split(
    "static void do_dump_options(",1)[0]
assert 'format("確定載入 %s？ ", ftmp)' in pref
assert 'format("Confirm loading %s? ", ftmp)' in pref
assert "process_pref_file(ftmp, false, true)" in pref
assert "player_safe_name(ftmp, sizeof(ftmp), player->full_name, true)" in pref
assert 'my_strcat(ftmp, ".prf", sizeof(ftmp))' in pref
assert "test_faangband_small_prompts_locale_source.py" in RUN
print("PASS: FA glyph color, keymap length, and pref load prompts bilingual")
