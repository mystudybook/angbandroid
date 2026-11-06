#!/usr/bin/env python3
"""Guard native quantity session completion and exact UI translations."""
from pathlib import Path
import unittest
import xml.etree.ElementTree as ET

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp"
JAVA = ROOT / "app/src/main/java/org/rephial/xyangband"
NPP = (CPP / "npp710/util.c").read_text(encoding="utf-8")
SILQ = (CPP / "silq/util.c").read_text(encoding="utf-8")
NPP054 = (CPP / "npp054/util.c").read_text(encoding="utf-8")
CLASSIFIER = (JAVA / "QuantityPromptText.java").read_text(encoding="utf-8")
POPUP = (JAVA / "QuantityPopup.java").read_text(encoding="utf-8")


def quantity_body(source):
    return source.split("s16b get_quantity(cptr prompt, int max)", 1)[1].split(
        "\n/*\n * Hack - duplication", 1
    )[0]


class VariantQuantitySessionTest(unittest.TestCase):
    def test_npp710_success_cancel_and_all_button_cleanup(self):
        body = quantity_body(NPP)
        self.assertEqual(1, body.count("Term_control_quantity(prompt, max, amt);"))
        self.assertEqual(2, body.count('Term_control(TERM_CONTROL_QUANTITY, "end");'))
        cancel = body.split("if (!get_string(prompt, buf, 7)) {", 1)[1].split(
            "return (0);", 1
        )[0]
        for required in ("soft_kbd_clear(true);", 'Term_control(TERM_CONTROL_QUANTITY, "end");',
                         "button_kill('a');", "event_signal(EVENT_MOUSEBUTTONS);"):
            self.assertIn(required, cancel)
        success = body.split("return (0);", 1)[1]
        self.assertLess(success.index('Term_control(TERM_CONTROL_QUANTITY, "end");'),
                        success.index("amt = atoi(buf);"))
        self.assertIn("if (p_ptr->command_arg)", body.split("Term_control_quantity", 1)[0])
        self.assertIn("button_kill('a');", success)
        self.assertIn("isalpha((unsigned char)buf[0])", success)

    def test_silq_success_cancel_and_repeat_preserved(self):
        body = quantity_body(SILQ)
        self.assertEqual(1, body.count("Term_control_quantity(prompt, max, amt);"))
        self.assertEqual(2, body.count('Term_control(TERM_CONTROL_QUANTITY, "end");'))
        cancel = body.split("if (!term_get_string(prompt, buf, 7)) {", 1)[1].split(
            "return (0);", 1
        )[0]
        self.assertIn("soft_kbd_clear(true);", cancel)
        self.assertIn('Term_control(TERM_CONTROL_QUANTITY, "end");', cancel)
        success = body.split("return (0);", 1)[1]
        self.assertLess(success.index('Term_control(TERM_CONTROL_QUANTITY, "end");'),
                        success.index("amt = atoi(buf);"))
        prefix = body.split("Term_control_quantity(prompt, max, amt);", 1)[0]
        self.assertIn("repeat_pull(&amt)", prefix)
        self.assertNotIn('Term_control(TERM_CONTROL_QUANTITY, "end");', prefix)
        self.assertIn("!isdigit((unsigned char)buf[0])", success)

    def test_npp054_accept_cancel_and_temporary_all_button(self):
        body = quantity_body(NPP054)
        # command_arg and max == 1 bypass the popup, exactly as before.
        self.assertIn("if (p_ptr->command_arg)", body.split(
            "Term_control_quantity(prompt, max, amt);", 1
        )[0])
        self.assertIn("else if ((max != 1))", body.split(
            "Term_control_quantity(prompt, max, amt);", 1
        )[0])
        self.assertEqual(1, body.count("Term_control_quantity(prompt, max, amt);"))
        self.assertEqual(2, body.count('Term_control(TERM_CONTROL_QUANTITY, "end");'))
        self.assertIn('soft_kbd_linger("${quant}*");', body)
        cancel = body.split("if (!get_string(prompt, buf, 7)) {", 1)[1].split(
            "return (0);", 1
        )[0]
        for required in ("soft_kbd_clear(true);",
                         'Term_control(TERM_CONTROL_QUANTITY, "end");',
                         "button_kill('a');", "event_signal(EVENT_MOUSEBUTTONS);"):
            self.assertIn(required, cancel)
        success = body.split("return (0);", 1)[1]
        self.assertLess(success.index('Term_control(TERM_CONTROL_QUANTITY, "end");'),
                        success.index("amt = atoi(buf);"))
        self.assertIn("button_kill('a');", success)
        self.assertIn("isalpha((unsigned char)buf[0])", success)
        # Reuses the exact existing NPP710 default prompt in Android's zh-TW UI.
        self.assertIn("Quantity (0-%d, '*' or 'a' = all): ", body)
        self.assertIn("R.string.quantity_prompt_npp_all", POPUP)

    def test_exact_generic_prompts_do_not_touch_game_input(self):
        self.assertIn("Quantity (0-%d, '*' or 'a' = all): ", NPP)
        self.assertIn("Quantity (0-%d): ", SILQ)
        for token, resource in (
            ("FROM_ZERO_NPP_ALL", "quantity_prompt_npp_all"),
            ("FROM_ZERO_SILQ", "quantity_prompt_silq"),
        ):
            self.assertIn("return " + token, CLASSIFIER)
            self.assertIn("case QuantityPromptText." + token, POPUP)
            self.assertIn("R.string." + resource, POPUP)
            for locale in ("values", "values-zh-rTW"):
                strings = {e.attrib["name"]: e.text or "" for e in
                           ET.parse(ROOT / "app/src/main/res" / locale / "strings.xml")
                           .getroot().findall("string")}
                self.assertIn(resource, strings)
                self.assertEqual(1, strings[resource].count("%1$d"))


if __name__ == "__main__":
    unittest.main()
