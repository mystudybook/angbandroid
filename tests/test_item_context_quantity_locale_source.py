#!/usr/bin/env python3
"""Source contract: CJK right-click item action popup and quantity retry state.

The popup's display text must not modify item IDs, confirmation, queued
commands, lore knowledge or the original English mode.
"""
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp/angband"
CONTEXT = (CPP / "ui-context.c").read_text(encoding="utf-8")
INPUT = (CPP / "ui-input.c").read_text(encoding="utf-8")
QUANTITY = (CPP / "quantity-input.h").read_text(encoding="utf-8")
LOCALE = (CPP / "zh-tw-ui.h").read_text(encoding="utf-8")
NATIVE = (ROOT / "tests/native/test_item_context_quantity.c").read_text(
    encoding="utf-8")
NATIVE_RUNNER = (ROOT / "scripts/test-native-utf8.sh").read_text(
    encoding="utf-8")


class ItemContextQuantityLocaleSourceTest(unittest.TestCase):
    def context(self):
        return CONTEXT.split("int context_menu_object(", 1)[1].split(
            "\n\nstatic int show_command_list(", 1)[0]

    def quantity(self):
        return INPUT.split("static int textui_get_quantity(", 1)[1].split(
            "\n/**\n * Verify something with the user", 1)[0]

    def test_context_popups_are_bounded_for_cjk_screen_geometry(self):
        c = self.context()
        self.assertIn('#include "menu-text-fit.h"', CONTEXT)
        self.assertIn("if (!obj || !Term || Term->wid < 8 || Term->hgt < 4)", c)
        self.assertIn("menu_dynamic_calc_location(m, Term->wid - 1, -1);", c)
        self.assertIn("r = m->boundary;", c)
        self.assertIn("area.width = -(r.width + 2);", c)
        self.assertIn("if (Term->wid - r.width >= 14)", c)
        self.assertIn("region_erase_bordered(&r);", c)
        self.assertNotIn("r.col = Term->wid - r.width - 1;", c)
        self.assertNotIn("r.page_rows = m->count;", c)
        self.assertNotIn("menu_layout(m, &r);", c)
        self.assertIn("char header[256];", c)
        self.assertIn("char prompt_text[512];", c)
        self.assertIn("char prompt_visible[512];", c)

    def test_lore_and_dynamic_prompt_stay_display_only(self):
        c = self.context()
        self.assertEqual(2, c.count("ODESC_PREFIX | ODESC_FULL | ODESC_ZH_TW"))
        self.assertIn("object_info(obj, OINFO_NONE)", c)
        self.assertNotIn("ODESC_SPOIL", c)
        self.assertIn("textui_textblock_place(tb, area,", c)
        self.assertIn("textui_textblock_show(tb, area,", c)
        self.assertIn("textblock_free(tb);", c)
        self.assertIn(
            'angdroid_game_ui_text("(Enter to select, ESC) Command for %s:",',
            c)
        self.assertIn("angdroid_zh_tw_enabled()), header);", c)
        self.assertIn("angdroid_menu_copy_columns(prompt_visible,", c)
        self.assertIn("prompt_text, (size_t)Term->wid);", c)
        self.assertIn("prt(prompt_visible, 0, 0);", c)
        self.assertIn(
            '{"(Enter to select, ESC) Command for %s:", '
            '"Enter 選取／Esc 返回：%s"}', LOCALE)

    def test_canceled_and_pseudo_actions_never_query_keymap(self):
        c = self.context()
        self.assertIn("case -1:", c)
        self.assertIn("return 3;", c)
        self.assertIn("case MENU_VALUE_INSPECT:", c)
        self.assertIn("case MENU_VALUE_DROP_ALL:", c)
        self.assertIn("cmd_set_arg_number(cmdq_peek(), \"quantity\", obj->number);", c)
        self.assertIn("cmdkey = cmd_lookup_key(selected, mode);", c)
        self.assertGreater(
            c.index("cmdkey = cmd_lookup_key(selected, mode);"),
            c.index("case CMD_USE:")
        )
        self.assertIn("key_confirm_command(cmdkey)", c)
        self.assertIn("get_item_allow(obj, cmdkey, selected, false)", c)
        self.assertIn("cmdq_push(selected);", c)
        self.assertIn("cmd_set_arg_item(cmdq_peek(), \"item\", obj);", c)
        self.assertIn("if (selected == CMD_DROP &&", c)
        self.assertIn("textui_cmd_ignore_menu(obj);", c)
        self.assertIn("return 2;", c)
        self.assertIn("menu_dynamic_free(m);", c)
        self.assertIn("string_free(labels);", c)

    def test_invalid_quantity_retains_previous_default(self):
        body = self.quantity()
        self.assertIn(
            "angdroid_parse_quantity_preserving_default(", body)
        self.assertIn("buf, max, &amt", body)
        self.assertNotIn("angdroid_parse_quantity(buf, max, &amt)", body)
        self.assertIn("int parsed = 0;", QUANTITY)
        self.assertIn("!angdroid_parse_quantity(input, max, &parsed)", QUANTITY)
        self.assertIn("*amount = parsed;", QUANTITY)
        self.assertIn("int amt = 1;", body)
        self.assertIn('strnfmt(buf, sizeof(buf), "%d", amt);', body)
        self.assertIn("if (!get_string(prompt, buf, sizeof(buf))) break;", body)
        self.assertIn("if (max <= 0) return 0;", body)
        self.assertIn("if (max == 1) return 1;", body)
        self.assertIn("Term_control_quantity(prompt, max, amt);", body)
        self.assertIn('Term_control(TERM_CONTROL_QUANTITY, "end");', body)
        self.assertIn("Invalid quantity. Enter a number or * for all.", body)
        self.assertIn("return result;", body)
        self.assertIn("tests/native/test_item_context_quantity.c", NATIVE_RUNNER)
        self.assertIn('quantity("oops", 25, 7, false, 7);', NATIVE)
        self.assertIn('quantity("*", 25, 7, true, 25);', NATIVE)
        self.assertIn("popup(w, h, labels[j], rows[k]);", NATIVE)


if __name__ == "__main__":
    unittest.main()
