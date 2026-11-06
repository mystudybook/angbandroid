#!/usr/bin/env python3
"""Translation-only inventory/equipment source guards.

Pure C99 formatters and native UTF-8 cell budgets are verified separately by
test_inventory_list_display.c, invoked from scripts/test-native-utf8.sh.
"""
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp/angband"
UI = (CPP / "ui-object.c").read_text(encoding="utf-8")
DISPLAY = (CPP / "inventory-list-display.h").read_text(encoding="utf-8")
STORE = (CPP / "ui-store.c").read_text(encoding="utf-8")
SHELL = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")


class InventoryListLocaleSourceTest(unittest.TestCase):
    def test_render_only_translation_preserves_sentinel_and_item_data(self):
        self.assertIn('#include "inventory-list-display.h"', UI)
        rendering = UI.split("static void show_obj(", 1)[1].split(
            "/**\n * ------------------------------------------------------------------------\n * Display of lists", 1)[0]
        self.assertIn('streq(display_label, "In quiver")', rendering)
        self.assertIn('display_label = "箭袋內";', rendering)
        self.assertIn("show_label ? display_label : \"\"", rendering)
        self.assertIn("angdroid_menu_copy_columns(label, sizeof(label),", rendering)
        # The English sentinel must not be replaced in cached selection data.
        self.assertIn('streq(items[i].label, "In quiver")', UI)
        self.assertIn('"In quiver");', UI)
        self.assertIn("angdroid_menu_copy_columns(name, sizeof(name),", rendering)
        self.assertNotIn('my_strcpy(items[obj_num].label, "箭袋內"', UI)

    def test_optional_columns_use_original_values_and_unchanged_widths(self):
        render = UI.split("static void show_obj(", 1)[1].split(
            "/**\n * ------------------------------------------------------------------------\n * Display of lists", 1)[0]
        self.assertEqual(4, render.count("object_list_put_detail(buf, row + obj_num, col,"))
        self.assertEqual(4, render.count("angdroid_inventory_detail_text(buf, sizeof(buf),"))
        for marker in ("ANGDROID_INVEN_PRICE", "ANGDROID_INVEN_DEVICE_FAIL",
                       "ANGDROID_INVEN_DEVICE_UNKNOWN", "ANGDROID_INVEN_RECHARGE_FAIL",
                       "ANGDROID_INVEN_WEIGHT"):
            self.assertIn(marker, render)
            self.assertIn(marker, DISPLAY)
        # No extra data is exposed before native knowledge checks.
        self.assertGreaterEqual(render.count("object_effect_is_known(obj)"), 2)
        self.assertIn("price_item(store, obj, true, obj->number)", render)
        self.assertIn("get_use_device_chance(obj)", render)
        self.assertIn("recharge_failure_chance(", render)
        self.assertIn("object_weight_one(obj)", render)
        self.assertIn("object_list_visible_extras(mode, Term->wid)", UI)
        self.assertIn("if (mode & OLIST_RECHARGE) ex_width += 10;", UI)
        self.assertIn("reserved, columns)", UI)

    def test_inventory_and_equipment_keep_stable_gear_and_quiver_semantics(self):
        self.assertIn("angdroid_inventory_burden_text(items[num_obj].label,", UI)
        self.assertIn("player->upkeep->total_weight, diff, angdroid_zh_tw_enabled()", UI)
        self.assertIn("angdroid_inventory_slot_text(buf, sizeof(buf), i,", UI)
        self.assertIn('strnfmt(tmp_val, sizeof(tmp_val), "箭袋：%d 支彈藥", count);', UI)
        self.assertIn("if (in_term) {", UI)
        self.assertIn("build_obj_list(player->body.count - 1, NULL, tester, mode);", UI)
        self.assertIn("build_obj_list(last_slot, player->upkeep->quiver, tester, mode);", UI)
        self.assertIn("m->selections = all_letters_nohjkl;", UI)
        self.assertIn('m->switch_keys = "/|-";', UI)
        self.assertIn("object_desc(items[i].o_name,", UI)

    def test_native_regression_and_store_transactions_unchanged(self):
        self.assertIn("tests/native/test_inventory_list_display.c", SHELL)
        self.assertIn("test_inventory_list_display", SHELL)
        self.assertIn("angdroid_store_quantity_prompt(o_name, sizeof(o_name),", STORE)
        self.assertIn('cmdq_push(CMD_BUY);', STORE)
        self.assertIn('cmdq_push(CMD_SELL);', STORE)
        self.assertIn('cmdq_push(CMD_STASH);', STORE)
        self.assertIn('cmdq_push(CMD_RETRIEVE);', STORE)


if __name__ == "__main__":
    unittest.main()
