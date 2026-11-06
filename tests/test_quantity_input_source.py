#!/usr/bin/env python3
"""Guard shared quantity parsing; exact parsing is exercised by a C99 test."""
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp/angband"
UI = (CPP / "ui-input.c").read_text(encoding="utf-8")
PARSER = (CPP / "quantity-input.h").read_text(encoding="utf-8")
LOCALE = (CPP / "zh-tw-ui.h").read_text(encoding="utf-8")
SHOP = (CPP / "ui-store.c").read_text(encoding="utf-8")


class QuantityInputSourceTest(unittest.TestCase):
    def body(self):
        return UI.split("static int textui_get_quantity(", 1)[1].split(
            "\n/**\n * Verify something with the user", 1)[0]

    def test_invalid_alpha_requires_retry_and_star_is_explicit(self):
        body = self.body()
        self.assertIn('#include "quantity-input.h"', UI)
        self.assertIn("angdroid_parse_quantity(buf, max, &amt)", body)
        self.assertIn("while (true)", body)
        self.assertIn("if (!get_string(prompt, buf, sizeof(buf))) break;", body)
        self.assertIn("if (*p == '*')", PARSER)
        self.assertIn("if (!digits) return false;", PARSER)
        self.assertIn("if (*p) return false;", PARSER)
        self.assertIn("value > (INT_MAX - digit) / 10", PARSER)
        self.assertNotIn("atoi(", body)
        self.assertNotIn("isalpha(", body)
        self.assertNotIn("buf[0] == '*'", body)

    def test_chinese_prompt_error_and_original_controls(self):
        body = self.body()
        self.assertIn("數量（0-%d，* 代表全部）：", body)
        self.assertIn("Quantity (0-%d, *=all): ", body)
        self.assertIn("Invalid quantity. Enter a number or * for all.", body)
        self.assertIn("數量格式無效，請輸入數字或 *（全部）。", LOCALE)
        self.assertIn("if (max <= 0) return 0;", body)
        self.assertIn("if (max == 1) return 1;", body)
        self.assertIn("Term_control_quantity(prompt, max, amt);", body)
        self.assertIn("soft_kbd_linger(", body)
        self.assertIn("return result;", body)
        self.assertIn("result = amt;", body)
        self.assertIn('Term_control(TERM_CONTROL_QUANTITY, "end");', body)
        self.assertEqual(1, body.count('Term_control(TERM_CONTROL_QUANTITY, "end");'))
        self.assertLess(body.index("Term_control_quantity(prompt, max, amt);"),
                        body.index('Term_control(TERM_CONTROL_QUANTITY, "end");'))

    def test_store_quantity_and_confirmation_remain_safe(self):
        self.assertIn("amt = get_quantity(ask, obj->number);", SHOP)
        self.assertIn("amt = get_quantity(o_name, amt);", SHOP)
        self.assertIn("if (amt <= 0) return false;", SHOP)
        self.assertIn("angdroid_store_explicit_yes(true, response.key.code)", SHOP)


if __name__ == "__main__":
    unittest.main()
