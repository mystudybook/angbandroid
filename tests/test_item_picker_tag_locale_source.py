#!/usr/bin/env python3
"""Original Angband item picker source contract: quiver tags, Esc, CJK titles."""
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp/angband"
UI = (CPP / "ui-object.c").read_text(encoding="utf-8")
LABELS = (CPP / "zh-tw-ui.h").read_text(encoding="utf-8")
GUARD = (CPP / "item-picker-tag-guard.h").read_text(encoding="utf-8")
NATIVE = (ROOT / "tests/native/test_item_picker_tag_guard.c").read_text(
    encoding="utf-8")
NATIVE_LOCALE = (ROOT / "tests/native/test_item_picker_cancel_locale.c").read_text(
    encoding="utf-8")
NATIVE_RUNNER = (ROOT / "scripts/test-native-utf8.sh").read_text(
    encoding="utf-8")


class ItemPickerTagLocaleSourceTest(unittest.TestCase):
    def test_quiver_tag_checks_array_capacity_before_access(self):
        block = UI.split("static bool get_tag(", 1)[1].split(
            "\n\n/**\n * ------------------------------------------------------------------------\n * Object selection menu", 1)[0]
        self.assertIn('#include "item-picker-tag-guard.h"', UI)
        self.assertIn("if (quiver_tags)", block)
        self.assertIn("angdroid_item_picker_quiver_index((unsigned char)tag,", block)
        self.assertIn("z_info->quiver_size)", block)
        self.assertIn("if (i >= 0 && player->upkeep->quiver[i])", block)
        self.assertNotIn("i = tag - '0';", block)
        self.assertIn("for (i = 0; i < num_obj; i++)", block)
        self.assertIn("strchr(quark_str(obj->note), '@')", block)
        self.assertIn("if (s[1] == tag)", block)
        self.assertIn("(s[1] == cmdkey) && (s[2] == tag)", block)
        self.assertIn("return false;", block)

    def test_quiver_index_bounds_and_inscription_fallback(self):
        self.assertIn("slot_count <= 0", GUARD)
        self.assertIn("tag < '0' || tag > '9'", GUARD)
        self.assertIn("index < slot_count ? index : -1;", GUARD)
        self.assertIn("test_tag('0' - 1, 10, -1)", NATIVE)
        self.assertIn("test_tag('9', 9, -1)", NATIVE)
        self.assertIn("test_tag(255, 10, -1)", NATIVE)
        self.assertIn("test_tag('9', 10, 9)", NATIVE)
        self.assertIn("tests/native/test_item_picker_tag_guard.c", NATIVE_RUNNER)
        self.assertIn("tests/native/test_item_picker_cancel_locale.c", NATIVE_RUNNER)
        self.assertIn('angdroid_game_ui_text(" ESC", 1)', NATIVE_LOCALE)
        self.assertIn('" Esc 取消"', NATIVE_LOCALE)

    def test_picker_escape_hint_is_display_only_bilingual(self):
        menu = UI.split("static void menu_header(void)", 1)[1].split(
            "\n\n/**\n * Get an item tag", 1)[0]
        self.assertIn('{" ESC", " Esc 取消"}', LABELS)
        self.assertIn('angdroid_game_ui_text(" ESC",', menu)
        self.assertIn("angdroid_zh_tw_enabled()", menu)
        self.assertIn('my_strcat(out_val,', menu)
        self.assertIn("static char header[288];", UI)
        self.assertIn("char out_val[256];", menu)
        self.assertIn('strnfmt(header, sizeof(header), "(%s)", out_val);', menu)
        self.assertIn('m->switch_keys = switch_keys;', UI)
        self.assertIn('angdroid_item_picker_switch_keys(', UI)
        self.assertNotIn('m->switch_keys = "/|-";', UI)
        self.assertIn("menu_select(m, 0, true);", UI)
        self.assertIn("if (evt.type == EVT_SWITCH && !newmenu)", UI)
        self.assertIn("return selection;", UI)
        self.assertIn("return (*choice != NULL) ? true : false;", UI)

    def test_examine_and_recall_title_buffers_support_cjk(self):
        self.assertEqual(UI.count("char header_buf[256];"), 3)
        self.assertNotIn("char header_buf[120];", UI)
        self.assertIn("void display_object_recall(struct object *obj)", UI)
        self.assertIn("void display_object_recall_interactive(struct object *obj)", UI)
        self.assertIn("void textui_obj_examine(void)", UI)
        self.assertIn("ODESC_PREFIX | ODESC_FULL | ODESC_ZH_TW, player);", UI)
        self.assertIn(
            "ODESC_PREFIX | ODESC_FULL | ODESC_CAPITAL | ODESC_ZH_TW, player);",
            UI)
        self.assertIn("object_info(obj, OINFO_NONE)", UI)
        self.assertIn("textblock_free(tb);", UI)
        self.assertIn("track_object(player->upkeep, obj);", UI)
        self.assertIn("if (!get_item(&obj, angdroid_game_ui_text(", UI)


if __name__ == "__main__":
    unittest.main()
