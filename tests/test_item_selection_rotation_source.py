#!/usr/bin/env python3
"""Rotated item menu coordinates and compact bilingual store prompt guards."""
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp/angband"
OBJECT = (CPP / "ui-object.c").read_text(encoding="utf-8")
MENU = (CPP / "ui-menu.c").read_text(encoding="utf-8")
STORE = (CPP / "ui-store.c").read_text(encoding="utf-8")
LAYOUT = (CPP / "item-selection-layout.h").read_text(encoding="utf-8")


class ItemSelectionRotationTest(unittest.TestCase):
    def test_rotated_menu_keeps_click_row_and_footer_separate(self):
        browser = OBJECT.split("static void item_menu_browser(", 1)[1].split(
            "static void item_menu_resize(", 1
        )[0]
        self.assertIn("angdroid_item_selection_footer_row(local_area->row,", browser)
        self.assertIn("if (line < 0 || i >= (int)strlen(all_letters_nohjkl)) break;", browser)
        self.assertIn("angdroid_item_quiver_footer_slots(", browser)
        self.assertIn("if (quiver_slots <= 0)", browser)
        self.assertNotIn("local_area->row + i", browser)
        self.assertIn("angdroid_menu_copy_columns(visible, sizeof(visible),", browser)
        self.assertIn("row = (int64_t)first_row + page_rows + index;", LAYOUT)
        self.assertIn("return row < terminal_height ? (int)row : -1;", LAYOUT)
        self.assertIn("angdroid_menu_scrolling_hit(", MENU)

    def test_selection_callback_rebuilds_geometry_after_rotation(self):
        self.assertIn("get_item_action, item_menu_resize };", OBJECT)
        self.assertIn("item_menu_resize(m);", OBJECT)
        self.assertIn("item_prompt_columns = prompt_size;", OBJECT)
        self.assertIn("area.width = layout.width;", OBJECT)
        self.assertIn("area.page_rows = layout.page_rows;", OBJECT)
        self.assertIn("angdroid_item_selection_region(width, height,", OBJECT)
        self.assertIn("menu_layout(m, &area);", OBJECT)
        self.assertIn("menu->preferred_skin == &menu_skin_object", MENU)
        self.assertIn("resized_before_calc = true;", MENU)
        self.assertIn("if (!menu_calc_size(menu))", MENU)
        self.assertIn("if (!resized_before_calc && menu->row_funcs->resize)", MENU)

    def test_shop_and_selection_prompts_clip_by_terminal_cells(self):
        self.assertIn("(int)angdroid_utf8_display_columns(pmt)", OBJECT)
        self.assertIn("int second_col = (int)left + 1;", OBJECT)
        self.assertIn("angdroid_menu_copy_columns(clipped, sizeof(clipped),", OBJECT)
        self.assertIn("wid >= 70 && hgt >= 16", STORE)
        self.assertIn("Term->wid >= 70 && Term->hgt >= 16", STORE)
        self.assertIn('"I 檢視  p 購買  d 出售  Esc 返回"', STORE)
        self.assertIn('"I 檢視  p 取出  d 存放  Esc 返回"', STORE)
        self.assertIn('"I 檢視  Esc 返回"', STORE)
        self.assertIn('format("Enter 選取／Esc 返回：%s", header)', STORE)
        self.assertIn("store_put_clipped(COLOUR_WHITE,", STORE)
        self.assertIn("context_menu_store_item(ctx, oid, 1, popup_row)", STORE)
        self.assertIn("oid, m->top, m->active.row, m->active.page_rows", STORE)


if __name__ == "__main__":
    unittest.main()
