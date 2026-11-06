#!/usr/bin/env python3
"""Android quantity popup input safety and zh-TW resources (source regression)."""
from pathlib import Path
import unittest
import xml.etree.ElementTree as ET

ROOT = Path(__file__).resolve().parents[1]
JAVA = ROOT / "app/src/main/java/org/rephial/xyangband"
RES = ROOT / "app/src/main/res"
POPUP = (JAVA / "QuantityPopup.java").read_text(encoding="utf-8")
ACTIVITY = (JAVA / "GameActivity.java").read_text(encoding="utf-8")
MATH = (JAVA / "QuantitySliderMath.java").read_text(encoding="utf-8")
LAYOUT = ET.parse(RES / "layout/quantity_layout.xml").getroot()
ANDROID = "{http://schemas.android.com/apk/res/android}"


class QuantityPopupSourceTest(unittest.TestCase):
    def test_quantity_changes_do_not_commit(self):
        self.assertEqual(1, POPUP.count("if (sendValue())"))
        self.assertIn("state.enqueueQuantity(", POPUP)
        self.assertNotIn("state.addKey(state.getKeyEnter());", POPUP)
        self.assertNotIn("state.addKey(state.getKeyBackspace());", POPUP)
        for name, suffix in (
            ("onProgressChanged", "public void updateValueBar"),
            ("onValueChange", None),
        ):
            block = POPUP.split("void " + name + "(", 1)[1]
            if suffix:
                block = block.split(suffix, 1)[0]
            self.assertNotIn("sendValue(", block)
        self.assertIn("if (locked || !fromUser) return;", POPUP)
        self.assertIn("if (sendValue()) {\n            resolved = true;", POPUP)

    def test_explicit_cancel_and_dismiss(self):
        self.assertIn("setFocusable(true);", POPUP)
        self.assertIn("setOutsideTouchable(false);", POPUP)
        self.assertIn("if (!resolved)", POPUP)
        self.assertIn("state.addKey(state.getKeyEsc());", POPUP)
        self.assertIn("id == R.id.cancelButton", POPUP)
        self.assertIn("id != R.id.confirmButton", POPUP)
        self.assertIn("quantPopup.dismiss();", ACTIVITY)
        self.assertIn("maxValue > 0", ACTIVITY)
        self.assertIn("catch (NumberFormatException ex)", ACTIVITY)

    def test_safe_slider(self):
        self.assertIn("QuantitySliderMath.valueForProgress(", POPUP)
        self.assertIn("QuantitySliderMath.progressForValue(", POPUP)
        self.assertIn("long range = (long) max - min;", MATH)
        self.assertIn("if (max <= min)", MATH)
        self.assertNotIn("progress * range", POPUP)
        self.assertNotIn("100 * newVal", POPUP)

    def test_layout_and_translations(self):
        ids = {element.get(ANDROID + "id") for element in LAYOUT.iter()}
        for name in ("quantityPrompt", "confirmButton", "cancelButton",
                     "allButton", "oneButton", "tenButton", "valuePicker", "valueBar"):
            self.assertIn("@+id/" + name, ids)
        for language in ("values", "values-zh-rTW"):
            root = ET.parse(RES / language / "strings.xml").getroot()
            table = {el.get("name"): el.text for el in root.findall("string")}
            for name in ("quantity_all_with_count", "quantity_confirm", "quantity_cancel"):
                self.assertIn(name, table)
                self.assertTrue(table[name])
            self.assertIn("%1$d", table["quantity_all_with_count"])
        self.assertIn("context.getString(R.string.quantity_all_with_count, limit)", POPUP)
        self.assertIn('prompt.setText(message == null ? "" : message);', POPUP)


if __name__ == "__main__":
    unittest.main()
