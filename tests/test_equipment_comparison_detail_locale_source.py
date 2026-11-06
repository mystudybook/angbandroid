#!/usr/bin/env python3
"""Source-only regression for the real native equipment comparison detail UI.

Compare detail is an on-screen view.  The sorting short_name, comparison
selection IDs, data tables, and file export must remain unlocalized so UI
translations cannot alter gameplay or change exported records.
"""
from pathlib import Path
import re
import unittest

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp/angband"
SOURCE = (CPP / "ui-equip-cmp.c").read_text(encoding="utf-8")
LABELS = (CPP / "zh-tw-equip-cmp.h").read_text(encoding="utf-8")
DESCRIPTOR = (CPP / "obj-desc.c").read_text(encoding="utf-8")
NATIVE_TEST = (ROOT / "tests/native/test_equip_cmp_locale.c").read_text(
    encoding="utf-8")
NATIVE_RUNNER = (ROOT / "scripts/test-native-utf8.sh").read_text(
    encoding="utf-8")


class EquipmentComparisonDetailLocaleSourceTest(unittest.TestCase):
    def test_comparison_header_is_translated_with_english_fallback(self):
        pairs = re.findall(r'\{\s*"([^"\n]+)"\s*,\s*"([^"\n]+)"\s*\}', LABELS)
        names = dict(pairs)
        self.assertEqual(len(pairs), len(names), "duplicate compare label key")
        self.assertEqual(names["Object comparison"], "裝備比較")
        self.assertIn('check("Object comparison", "裝備比較");', NATIVE_TEST)
        self.assertIn('if (!english || !zh_tw) return english;', LABELS)
        self.assertIn('return english;', LABELS)

    def test_two_item_details_use_localized_item_descriptions_only_on_screen(self):
        details = SOURCE.split(
            "static void display_object_comparison(", 1)[1].split(
            "\n\nstatic bool dump_to_file(", 1)[0]
        self.assertIn("char hbuf[256];", details)
        self.assertIn("ODESC_PREFIX | ODESC_FULL | ODESC_CAPITAL |", details)
        self.assertIn("(angdroid_zh_tw_enabled() ? ODESC_ZH_TW : 0)", details)
        self.assertEqual(2, details.count("description_mode, p);"))
        self.assertIn("object_info(s->items[s->isel0].obj, OINFO_NONE)", details)
        self.assertIn("object_info(s->items[s->isel1].obj, OINFO_NONE)", details)
        self.assertIn('angdroid_equip_cmp_text("Object comparison",', details)
        self.assertIn("angdroid_zh_tw_enabled()", details)
        self.assertIn("textui_textblock_show(tb0, local_area, hbuf)", details)
        self.assertIn("textui_textblock_show(tb1, local_area,", details)
        self.assertIn("textblock_free(tb0);", details)
        self.assertIn("textblock_free(tb1);", details)
        self.assertNotIn("ODESC_SPOIL", details)

    def test_knowledge_gates_and_canonical_comparison_data_stay_intact(self):
        self.assertIn("(mode & ODESC_ZH_TW) && angdroid_zh_tw_enabled()", DESCRIPTOR)
        self.assertIn("show_flavor && obj->kind->flavor", DESCRIPTOR)
        self.assertIn("aware &&", DESCRIPTOR)
        self.assertIn("object_desc(buf, N_ELEMENTS(buf), obj, ODESC_COMBAT |", SOURCE)
        self.assertIn("ODESC_SINGULAR | ODESC_TERSE, p);", SOURCE)
        self.assertIn("return strcmp(left->short_name, right->short_name);", SOURCE)
        self.assertIn("Term_putstr(s->icol_name, rdetails.value_position.y,", SOURCE)
        self.assertIn("e->nmlen, nmcolor, e->short_name);", SOURCE)
        self.assertIn('file_putf(fff, "%s\\n", buf);', SOURCE)
        self.assertIn("Term_what(x, y, &a, &c)", SOURCE)
        self.assertIn("s->items[i].short_name =", SOURCE)
        self.assertIn("set_short_name(s->items[i].obj, length, p);", SOURCE)

    def test_existing_offline_c99_test_keeps_new_assertion(self):
        self.assertIn('tests/native/test_equip_cmp_locale.c', NATIVE_RUNNER)
        self.assertIn('test_equip_cmp_locale', NATIVE_RUNNER)
        self.assertIn('check("Unsupported future label", "Unsupported future label");',
                      NATIVE_TEST)
        self.assertIn("angdroid_equip_cmp_text(NULL, 1) == NULL", NATIVE_TEST)


if __name__ == "__main__":
    unittest.main()
