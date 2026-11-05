#!/usr/bin/env python3
"""Store stock-generation invalidation and Enter safety (source contracts).

These guards do not claim to simulate the Android touch event queue.
"""
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp/angband"
STORE = (CPP / "ui-store.c").read_text(encoding="utf-8")
MENU = (CPP / "ui-menu.c").read_text(encoding="utf-8")
HEADER = (CPP / "store-stock-guard.h").read_text(encoding="utf-8")
INPUT = (CPP / "ui-input.c").read_text(encoding="utf-8")


def section(text, before, after):
    return text.split(before, 1)[1].split(after, 1)[0]


class StoreInputInvalidationTest(unittest.TestCase):
    def test_mutation_invalidates_previous_stock_id_and_drains_queue(self):
        callback = section(STORE,
            "static void refresh_stock(game_event_type type,",
            "\n/**\n * Enter a store.")
        self.assertLess(callback.index("store_stock_list(ctx->store,"),
                        callback.index("ctx->stock_changed = true;"))
        self.assertLess(callback.index("ctx->stock_changed = true;"),
                        callback.index("flush(type, unused, user);"))
        self.assertLess(callback.index("flush(type, unused, user);"),
                        callback.index("store_menu_recalc(menu);"))
        self.assertIn("if (inkey_xtra) {", INPUT)
        self.assertIn("Term_flush();", INPUT)

        recalc = section(STORE, "static void store_menu_recalc(",
                         "\n/**\n * Process a command")
        self.assertIn("menu_setpriv(m, ctx->store->stock_num, ctx);", recalc)
        self.assertIn("angdroid_store_clear_stale_selection(ctx->stock_changed,",
                      recalc)
        self.assertLess(recalc.index("menu_setpriv("),
                        recalc.index("angdroid_store_clear_stale_selection("))
        self.assertIn("if (cursor) *cursor = -1;", HEADER)
        self.assertIn("if (top) *top = 0;", HEADER)
        self.assertIn("if (!stock_changed) return;", HEADER)

    def test_new_command_acknowledges_rebuilt_stock(self):
        initializer = section(STORE, "static void store_menu_init(",
                              "\n/**\n * Display contents of a store")
        self.assertIn("ctx->stock_changed = false;", initializer)
        handler = section(STORE, "static bool store_menu_handle(",
                          "\n/* Recalculate price/weight")
        self.assertIn("ctx->stock_changed = false;", handler)
        self.assertLess(handler.index("ctx->stock_changed = false;"),
                        handler.index("if (event->type == EVT_SELECT)"))
        self.assertIn("region_inside(&m->active, event)", handler)
        self.assertIn("angdroid_store_stock_index_valid(", handler)
        self.assertIn("oid, m->top, m->active.row, m->active.page_rows", handler)

    def test_enter_cannot_activate_an_invalid_cursor(self):
        keypress = section(MENU, "bool menu_handle_keypress(",
                           "\nstatic void keys_to_ui(")
        enter = section(keypress, "} else if (in->key.code == KC_ENTER) {",
                        "} else {")
        self.assertIn("if (is_valid_row(menu, menu->cursor))", enter)
        self.assertIn("out->type = EVT_SELECT;", enter)
        self.assertIn("eat = true;", enter)
        self.assertNotIn("out->type = EVT_SELECT;\n\t} else {", keypress)

    def test_unmodified_back_double_tap_and_keyboard_command_paths(self):
        mouse = section(MENU, "bool menu_handle_mouse(",
                        "\n/**\n * Handle any menu command")
        self.assertIn("if (in->mouse.button == 2)", mouse)
        self.assertIn("new_cursor == menu->cursor || !(menu->flags & MN_DBL_TAP)", mouse)
        select = section(MENU, "ui_event menu_select(",
                         "\n/* ================== MENU ACCESSORS")
        self.assertIn("menu_handle_mouse(menu, &in, &out);", select)
        self.assertIn("menu_handle_action(menu, &in)", select)
        self.assertIn("if (out.type == EVT_SELECT && !no_act", select)


if __name__ == "__main__":
    unittest.main()
