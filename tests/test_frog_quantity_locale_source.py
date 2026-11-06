#!/usr/bin/env python3
"""FrogComposband native quantity lifetime + display-only zh-TW localization."""
from pathlib import Path
import unittest
import xml.etree.ElementTree as ET

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp"
JAVA = ROOT / "app/src/main/java/org/rephial/xyangband"
RES = ROOT / "app/src/main/res"
FROG = (CPP / "frogcomposband/util.c").read_text(encoding="utf-8")
ANG = (CPP / "angband/ui-input.c").read_text(encoding="utf-8")
TRANSLATIONS = (CPP / "angband/zh-tw-ui.h").read_text(encoding="utf-8")
POPUP = (JAVA / "QuantityPopup.java").read_text(encoding="utf-8")
PROMPTS = (JAVA / "QuantityPromptText.java").read_text(encoding="utf-8")


class FrogQuantityLocaleSourceTest(unittest.TestCase):
    def frog_body(self):
        return FROG.split("s16b get_quantity_aux(", 1)[1].split(
            "\n/*\n * Request a \"quantity\" from the user\n */\ns16b get_quantity(", 1
        )[0]

    def test_frog_ends_native_session_on_accept_and_cancel(self):
        body = self.frog_body()
        self.assertEqual(1, body.count("Term_control_quantity(prompt, max, amt);"))
        self.assertEqual(1, body.count('Term_control(TERM_CONTROL_QUANTITY, "end");'))
        self.assertLess(body.index("Term_control_quantity(prompt, max, amt);"),
                        body.index('Term_control(TERM_CONTROL_QUANTITY, "end");'))
        self.assertLess(body.index("res = askfor_aux(buf, 7, FALSE);"),
                        body.index('Term_control(TERM_CONTROL_QUANTITY, "end");'))
        self.assertLess(body.index('Term_control(TERM_CONTROL_QUANTITY, "end");'),
                        body.index("if (!res) return 0;"))
        self.assertIn("soft_kbd_clear(true);", body)

    def test_frog_quantity_semantics_and_non_prompt_paths_unchanged(self):
        body = self.frog_body()
        # These return paths do not open a popup and must not send end.
        prefix = body.split("Term_control_quantity(prompt, max, amt);", 1)[0]
        self.assertIn("if (command_arg)", prefix)
        self.assertIn("repeat_pull(&amt)", prefix)
        self.assertNotIn('Term_control(TERM_CONTROL_QUANTITY, "end");', prefix)
        self.assertIn('sprintf(tmp, "Quantity (1-%d): ", max);', body)
        self.assertIn("amt = MAX(1, MIN(default_amt, max));", body)
        self.assertIn("amt = atoi(buf);", body)
        self.assertIn("if (isalpha(buf[0])) amt = max;", body)
        self.assertIn("if (amt > max) amt = max;", body)
        self.assertIn("if (amt < 0) amt = 0;", body)
        self.assertIn("if (amt) repeat_push(amt);", body)

    def test_only_exact_generic_prompts_can_be_localized(self):
        self.assertIn('prompt.equals("Quantity (1-" + max + "): ")', PROMPTS)
        self.assertIn('prompt.equals("Quantity (0-" + max + ", *=all): ")', PROMPTS)
        self.assertIn("max <= 0", PROMPTS)
        self.assertIn("switch (QuantityPromptText.classify(promptText, limit))", POPUP)
        self.assertIn("prompt.setText(promptText);", POPUP)
        self.assertIn("R.string.quantity_prompt_from_one", POPUP)
        self.assertIn("R.string.quantity_prompt_from_zero_all", POPUP)
        self.assertIn("sessionToken, valuePicker.getValue(), valuePicker.getMaxValue()", POPUP)

    def test_translations_preserve_numeric_format_parameters(self):
        for locale in ("values", "values-zh-rTW"):
            table = {v.attrib["name"]: v.text or "" for v in
                     ET.parse(RES / locale / "strings.xml").getroot().findall("string")}
            for label in ("quantity_prompt_from_one", "quantity_prompt_from_zero_all"):
                self.assertIn(label, table)
                self.assertEqual(1, table[label].count("%1$d"))
        self.assertIn('{"Repeat: %d", "重複次數：%d"}', TRANSLATIONS)
        self.assertIn('{"Saving as %s.", "正在儲存為 %s。"}', TRANSLATIONS)

    def test_angband_uses_existing_display_only_mapping(self):
        self.assertIn('angdroid_game_ui_text("Repeat: %d",', ANG)
        self.assertIn('angdroid_game_ui_text("Saving as %s.",', ANG)
        self.assertIn("angdroid_zh_tw_enabled()), count)", ANG)
        self.assertIn("angdroid_zh_tw_enabled()), path)", ANG)


if __name__ == "__main__":
    unittest.main()
