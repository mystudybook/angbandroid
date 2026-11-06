#!/usr/bin/env python3
"""Display-only zh-TW version header and item-ignore picker source checks."""
import re
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp/angband"
VERSION = (CPP / "ui-command.c").read_text(encoding="utf-8")
ITEM = (CPP / "ui-object.c").read_text(encoding="utf-8")
DICT = (CPP / "zh-tw-ui.h").read_text(encoding="utf-8")
HEADER = (CPP / "zh-tw-version-header.h").read_text(encoding="utf-8")
STORE = (CPP / "ui-store.c").read_text(encoding="utf-8")
QT = (CPP / "ui-input.c").read_text(encoding="utf-8")
NATIVE_RUNNER = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")


class VersionIgnorePickSourceTest(unittest.TestCase):
    def test_version_display_uses_original_buildver_and_copyright(self):
        version = VERSION.split("void do_cmd_version(void)", 1)[1].split(
            "void textui_cmd_retire(void)", 1)[0]
        self.assertIn('#include "zh-tw-version-header.h"', VERSION)
        self.assertIn("char header_buf[256];", version)
        self.assertIn("angdroid_version_header_text(header_buf, sizeof(header_buf),", version)
        self.assertIn("buildver, angdroid_zh_tw_enabled()", version)
        self.assertIn('angdroid_game_ui_text("Version information",', version)
        self.assertIn('textblock_append(tb, "%s", copyright);', version)
        self.assertIn("textui_textblock_show(tb, local_area, header_buf);", version)
        self.assertIn("textblock_free(tb);", version)
        self.assertNotIn("buildver =", version)
        self.assertIn('snprintf(out, capacity, "正在遊玩 %s。按 ? 查看詳細資訊。", version)', HEADER)
        self.assertIn('"You are playing %s.  Type \'?\' for more info."', HEADER)
        self.assertIn("if (written < 0 || (size_t)written >= capacity)", HEADER)
        self.assertIn("out[0] = '\\0';", HEADER)

    def test_item_ignore_picker_translates_only_prompt_not_selection(self):
        picker = ITEM.split("void textui_cmd_ignore(void)", 1)[1].split(
            "void textui_cmd_toggle_ignore(void)", 1)[0]
        self.assertIn('angdroid_game_ui_text(\n\t\t"Ignore which item? "', picker)
        self.assertIn('angdroid_game_ui_text(\n\t\t"You have nothing to ignore."', picker)
        self.assertIn("CMD_IGNORE, NULL,", picker)
        self.assertIn("USE_INVEN | USE_QUIVER | USE_EQUIP | USE_FLOOR", picker)
        self.assertIn("textui_cmd_ignore_menu(obj);", picker)
        self.assertIn("if (!get_item(&obj, q, s,", picker)
        self.assertIn("return;", picker)
        self.assertIn("player->unignoring = !player->unignoring;", ITEM)

    def test_mapping_remains_exact_and_bilingual(self):
        pairs = re.findall(r'\{\s*"([^"\n]+)"\s*,\s*"([^"\n]+)"\s*\}', DICT)
        names = dict(pairs)
        self.assertEqual(len(pairs), len(names), "duplicate dictionary keys")
        self.assertEqual(names["Ignore which item? "], "要忽略哪件物品？ ")
        self.assertEqual(names["You have nothing to ignore."], "沒有可以忽略的物品。")
        self.assertEqual(names["Version information"], "遊戲版本資訊")
        self.assertIn("if (!zh_tw || !en) return en;", DICT)
        self.assertIn("return en;", DICT)

    def test_quantity_popup_and_store_pick_untouched(self):
        self.assertIn("angdroid_store_quantity_prompt(quantity_prompt,", STORE)
        self.assertIn("angdroid_store_quantity_prompt(o_name, sizeof(o_name),", STORE)
        self.assertIn("amt = get_quantity(o_name, amt);", STORE)
        self.assertIn("Term_control_quantity(prompt, max, amt);", QT)
        self.assertIn('Term_control(TERM_CONTROL_QUANTITY, "end");', QT)
        self.assertIn("tests/native/test_version_header_text.c", NATIVE_RUNNER)


if __name__ == "__main__":
    unittest.main()
