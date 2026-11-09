#!/usr/bin/env python3
"""Equipment slot mention widths must be terminal-cell safe, not codepoint based."""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
def read(path):
    return (ROOT / path).read_text(encoding="utf-8")

UI = read("app/src/main/cpp/angband/ui-object.c")
H = read("app/src/main/cpp/angband/inventory-equip-mention.h")
FIT = read("app/src/main/cpp/angband/menu-text-fit.h")
TEST = read("tests/native/test_inventory_equip_mention.c")
NATIVE = read("scripts/test-native-utf8.sh")
VERIFY = read("scripts/verify-zh-tw.sh")

start = UI.split("static void build_obj_list(", 1)[1].split(
    "static void set_obj_names(", 1)[0]
assert '#include "inventory-equip-mention.h"' in UI
assert "const char *mention = equip_mention(player, i);" in start
assert "angdroid_inventory_equip_mention(" in start
assert "sizeof(items[num_obj].equip_label), mention);" in start
assert "utf8_strlen(mention)" not in start
assert "utf8_clipto(mention_copy, 14)" not in start
assert "strnfmt(buf, sizeof(buf), \"%s%*s\"" not in start
assert "items[num_obj].object = obj;" in start
assert "items[num_obj].key = (items[num_obj].label)[0];" in start
assert "angdroid_inventory_slot_text(buf, sizeof(buf), i," in start
assert "sizeof(items[num_obj].equip_label)" in start
assert "target_columns = 14" in H
assert "angdroid_menu_copy_columns(dest, capacity, mention," in H
assert "while (columns < target_columns && bytes + 1 < capacity)" in H
assert "if (!mention) return 0;" in H
assert "angdroid_utf8_next_scalar(source, &codepoint)" in FIT
assert "angdroid_terminal_columns(codepoint)" in FIT
assert "if (length >= capacity - bytes) break;" in FIT

for text in ("abcdefghijklmn", "頭部          ", "一二三四五六七",
             "頭盔 Helm of E", "武器 (weapon) "):
    assert text in TEST
assert "char tiny[3], exact[4], partial[8]" in TEST
assert "tests/native/test_inventory_equip_mention.c" in NATIVE
assert "tests/test_inventory_equip_mention_source.py" in VERIFY
print("PASS: equipment slot mention label respects 14 terminal columns")
