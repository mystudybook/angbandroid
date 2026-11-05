#!/usr/bin/env python3
"""Source guards for explicit monetary consent, store labels and one-touch dispatch."""
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp/angband"
STORE = (CPP / "ui-store.c").read_text(encoding="utf-8")
MENU = (CPP / "ui-menu.c").read_text(encoding="utf-8")
INPUT = (CPP / "ui-input.c").read_text(encoding="utf-8")
HEADER = (CPP / "store-confirmation-text.h").read_text(encoding="utf-8")

def section(text, begin, end):
    return text.split(begin, 1)[1].split(end, 1)[0]

class StoreConfirmationSourceTest(unittest.TestCase):
    def test_confirmation_accepts_explicit_y_only(self):
        confirm = section(STORE, "static bool store_get_check(",
                          "\n/*\n * Sell an object")
        self.assertIn("soft_kbd_flash(", confirm)
        self.assertIn("yes_no", confirm)
        self.assertIn("flush((game_event_type)0, NULL, NULL);", confirm)
        self.assertLess(confirm.index("flush((game_event_type)0, NULL, NULL);"),
                        confirm.index("response = inkey_m();"))
        self.assertIn("store_put_clipped(COLOUR_WHITE, prompt,", confirm)
        self.assertIn("if (response.type != EVT_KBRD) return false;", confirm)
        self.assertIn("return angdroid_store_explicit_yes(true, response.key.code);", confirm)
        self.assertNotIn("strchr(", confirm)
        self.assertNotIn("ch = inkey()", confirm)
        self.assertIn("keyboard && (code == 'y' || code == 'Y')", HEADER)
        self.assertIn("ke.key.code = '\\n';", INPUT)

    def test_buy_sell_take_stash_quantity_and_names_translated(self):
        sale = section(STORE, "static bool store_sell(",
                       "\n/**\n * Buy an object")
        buy = section(STORE, "static bool store_purchase(",
                      "\n/**\n * Examine an item")
        popup = section(STORE, "static bool context_menu_store_item(",
                        "\n/**\n * Handle store menu input")
        for action in ("STASH", "GIVE", "SELL"):
            self.assertIn("ANGDROID_STORE_Q_" + action, sale)
        for action in ("BUY", "TAKE"):
            self.assertIn("ANGDROID_STORE_Q_" + action, buy)
        self.assertIn("angdroid_store_quantity_prompt(quantity_prompt,", sale)
        self.assertIn("angdroid_store_quantity_prompt(o_name, sizeof(o_name),", buy)
        self.assertIn("amt = get_quantity(ask, obj->number);", sale)
        self.assertIn("amt = get_quantity(o_name, amt);", buy)
        self.assertIn("if (amt <= 0) return false;", buy)
        self.assertIn("ODESC_PREFIX | ODESC_FULL | ODESC_ZH_TW", sale)
        self.assertIn("ODESC_PREFIX | ODESC_FULL | ODESC_STORE | ODESC_ZH_TW", buy)
        self.assertIn("ODESC_PREFIX | ODESC_FULL | ODESC_ZH_TW |", popup)
        self.assertIn('"[Y 確認／N 或 Esc 取消] %s %s？"', sale)
        self.assertIn('"[Y 確認／N 或 Esc 取消] 購買 %s？%s"', buy)
        self.assertIn("[Y confirm / N or Esc cancel]", sale)
        self.assertIn("[Y confirm / N or Esc cancel]", buy)
        self.assertIn("if (!response) {", buy)
        self.assertIn("object_delete(NULL, NULL, &dummy);", buy)
        self.assertIn("if (!store_get_check(", sale)
        self.assertIn("response = store_get_check(", buy)

    def test_active_touch_dispatches_callback_once_even_if_false(self):
        select = section(MENU, "ui_event menu_select(",
                         "\n/* ================== MENU ACCESSORS")
        mouse = section(select, "if (in.type == EVT_MOUSE) {",
                        "} else if (in.type == EVT_KBRD)")
        self.assertIn("bool touch_select_dispatched = false;", select)
        self.assertLess(mouse.index("menu_handle_mouse(menu, &in, &out);"),
                        mouse.index("touch_select_dispatched = true;"))
        self.assertEqual(mouse.count("touch_select_dispatched = true;"), 1)
        self.assertIn("if (menu_handle_action(menu, &in))", mouse)
        self.assertIn("!touch_select_dispatched &&", select)
        self.assertIn("menu_handle_action(menu, &out))", select)
        self.assertIn("if (!no_act && menu_handle_action(menu, &in))", mouse)

if __name__ == "__main__":
    unittest.main()
