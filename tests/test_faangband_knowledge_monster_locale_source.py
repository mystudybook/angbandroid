#!/usr/bin/env python3
"""FA knowledge UI: localize summaries/footer without changing gameplay IDs."""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp"
source = (CPP / "faangband/ui-knowledge.c").read_text(encoding="utf-8")
table = (CPP / "angband/zh-tw-ui.h").read_text(encoding="utf-8")
runner = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")
verify = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")
helper = (CPP / "angband/zh-tw-knowledge-kill-summary.h").read_text(
    encoding="utf-8")

def block(start, finish):
    return source.split(start, 1)[1].split(finish, 1)[0]

browser = block("static void display_knowledge(", "static void display_monster(")
monster = block("static void display_monster(", "static int m_cmp_race(")
summary = block("static void mon_summary(", "static int count_known_monsters(")
recall = block("static const char *recall_prompt(", "#define swap(")

# Native keycodes, lore, sort/index logic and menu selection remain intact.
assert 'soft_kbd_flash("r");' in recall
assert 'return ", \'r\' to recall";' in recall
assert "soft_kbd_clear(true);" in browser
assert 'soft_kbd_flash("c");' in browser
assert 'soft_kbd_flash("p");' in browser
assert 'soft_kbd_flash("i");' in browser
assert 'soft_kbd_flash("v");' in browser
assert "angdroid_game_ui_text(pvs, true)" in browser
assert "angdroid_game_ui_text(pedit, true)" in browser
assert "angdroid_game_ui_text(xtra, true)" in browser
assert '"方向鍵移動%s%s%s，Esc 返回"' in browser
assert 'if (wid < 12 || hgt < 9)' in browser
assert '"知識圖鑑：%s"' in browser
assert '"Knowledge - %s"' in browser
assert "angdroid_knowledge_clip_label(visible_title," in browser
assert "angdroid_utf8_display_columns(full_prompt)" in browser
assert '"Esc 返回" : "Esc"' in browser
assert "my_strcpy(full_prompt, short_prompt, sizeof(full_prompt));" in browser
assert "angdroid_knowledge_clip_label(visible_prompt," in browser
assert "if (angdroid_zh_tw_enabled())" in browser
assert browser.index("soft_kbd_clear(true);") < browser.index("ke = inkey_ex();")
assert "ke = inkey_ex();\n        soft_kbd_clear(true);" in browser

# Historical fixed-width 80-column lines must respect the Android terminal.
assert "for (i = 0; i < MIN(79, wid); i++)" in browser
assert "g_name_len = MIN(g_name_len, MAX(1, wid - 11));" in browser
assert "fa_knowledge_header_field(" in browser
assert "fa_knowledge_header_field(\"擊殺\", 70);" in browser
assert "fa_knowledge_header_field(\"符號\", 65);" in browser
assert "angdroid_knowledge_clip_label(clipped, sizeof(clipped)," in browser
assert "目前沒有已知%s。" in browser
assert 'angdroid_game_ui_text(title, true)' in browser

# Keep all original monster counts; change visible strings and safety only.
assert "lore->pkills" in monster
assert "monster_x_attr[race->ridx]" in monster
assert "angdroid_knowledge_monster_label(" in monster
assert "angdroid_knowledge_clip_label(clipped, sizeof(clipped)," in monster
assert "if (Term && Term->wid > 66) big_pad(66, row, a, c);" in monster
assert "if (Term && Term->wid > 70)" in monster
for value in ('"shape"', '" dead"', '"alive"'):
    assert value in monster and value in table
assert "kills += l_list[oid].pkills;" in summary
assert "tkills += l_list[i].pkills;" in summary
assert "gid == 0 && n > 0" in summary
assert "item_list[top]" in summary
assert "angdroid_knowledge_kill_summary(" in summary
assert "angdroid_knowledge_clip_label(clipped, sizeof(clipped)," in summary

assert "已知特殊怪物 %d 隻，已擊敗 %d 隻。" in helper
assert "擊敗怪物：本類 %d／全部 %d" in helper
assert "known uniques, %d slain" in helper
assert "Creatures slain: %d/%d (in group/in total)" in helper
assert "test_knowledge_kill_summary_locale.c" in runner
assert "test_faangband_knowledge_monster_locale_source.py" in verify
print("PASS: FA monster knowledge strings, footer buttons and column guards")
