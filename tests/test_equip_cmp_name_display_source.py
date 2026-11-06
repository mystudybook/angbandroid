#!/usr/bin/env python3
"""Source guard for CJK equipment-compare ROW names without touching export.

Normal redraw calls display_page(..., true); the file writer calls it with
false. Translating the object list cache would alter sorting and exports.
"""
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp/angband"
SOURCE = (CPP / "ui-equip-cmp.c").read_text(encoding="utf-8")
HELPER = (CPP / "equip-compare-name-display.h").read_text(encoding="utf-8")
DESC = (CPP / "obj-desc.c").read_text(encoding="utf-8")
TEST = (ROOT / "tests/native/test_equip_cmp_name_display.c").read_text(
    encoding="utf-8")
RUN = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")


class EquipComparisonScreenNameSourceTest(unittest.TestCase):
    def test_name_generator_obeys_native_knowledge_gates(self):
        block = SOURCE.split("static bool equip_cmp_screen_short_name(", 1)[1].split(
            "\n\nstatic int display_page(", 1)[0]
        self.assertIn('#include "equip-compare-name-display.h"', SOURCE)
        self.assertIn('#include "zh-tw-artifact-names.h"', SOURCE)
        self.assertIn('#include "zh-tw-ego-names.h"', SOURCE)
        self.assertIn("e->obj->known && e->obj->known->artifact", block)
        self.assertIn("e->obj->known && e->obj->known->ego", block)
        self.assertIn("angdroid_zh_tw_artifact_name(visible)", block)
        self.assertIn("angdroid_zh_tw_ego_name(visible)", block)
        self.assertIn("ODESC_COMBAT | ODESC_SINGULAR | ODESC_TERSE | ODESC_ZH_TW", block)
        self.assertIn("keep_tail = true;", block)
        self.assertIn("angdroid_equip_cmp_format_short_name(out, capacity, visible,", block)
        self.assertNotIn("ODESC_SPOIL", block)
        self.assertIn("(mode & ODESC_ZH_TW) && angdroid_zh_tw_enabled()", DESC)

    def test_display_only_branch_never_changes_sorting_or_export(self):
        display = SOURCE.rsplit("static int display_page(", 1)[1]
        self.assertIn("if (allow_reconfig && angdroid_zh_tw_enabled() && s->nshortnm > 0)", display)
        self.assertIn("equip_cmp_screen_short_name(display_name,", display)
        self.assertIn("-1, nmcolor, display_name);", display)
        self.assertIn("e->nmlen, nmcolor, e->short_name);", display)
        self.assertIn("rdetails.value_position.x = s->icol_name + s->nshortnm + 1;", display)
        self.assertIn("display_page(the_summary, player, true)", SOURCE)
        self.assertEqual(2, SOURCE.count("display_page(s_for_file, player, false)"))
        self.assertIn("return strcmp(left->short_name, right->short_name);", SOURCE)
        self.assertIn("e->short_name = set_short_name(obj, c->summary->nshortnm,", SOURCE)
        self.assertIn("set_short_name(s->items[i].obj, length, p);", SOURCE)
        self.assertIn("e->nmlen = (int)strlen(e->short_name);", SOURCE)
        self.assertIn('file_putf(fff, "%s\\n", buf);', SOURCE)
        self.assertIn("Term_what(x, y, &a, &c);", SOURCE)
        self.assertIn("s_for_file->npage = cached_npage;", SOURCE)
        self.assertIn("s_for_file->ifirst = cached_ifirst;", SOURCE)

    def test_column_clipping_tail_padding_and_overflow(self):
        self.assertIn('#include "menu-text-fit.h"', HELPER)
        self.assertIn("angdroid_utf8_display_columns(name)", HELPER)
        self.assertIn("angdroid_utf8_skip_columns(name, total - columns)", HELPER)
        self.assertIn("angdroid_menu_copy_columns(out, capacity, source, columns)", HELPER)
        self.assertIn("while (used < columns)", HELPER)
        self.assertIn("if (length >= capacity - 1)", HELPER)
        self.assertIn("out[0] = '\\0';", HELPER)
        self.assertIn('verify("前綴天地玄黃", 6, true, "地玄黃")', TEST)
        self.assertIn('verify("火焰之劍", 7, false, "火焰之 ")', TEST)
        self.assertIn('verify("Ring of Power", 7, true, "f Power")', TEST)
        self.assertIn("tests/native/test_equip_cmp_name_display.c", RUN)


if __name__ == "__main__":
    unittest.main()
