#!/usr/bin/env python3
"""Regression guards for equipment compare's failed/zero-size terminal paths.

A tiny terminal can cause initialize_summary() to return before item counting,
filtering or sorting. The BAIL state must not read those fields.
"""
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp/angband"
SOURCE = (CPP / "ui-equip-cmp.c").read_text(encoding="utf-8")
HELPER = (CPP / "equip-compare-state-guard.h").read_text(encoding="utf-8")
RUNNER = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")
NATIVE = (ROOT / "tests/native/test_equip_cmp_tiny_terminal.c").read_text(
    encoding="utf-8")


class EquipCmpTinyTerminalSourceTest(unittest.TestCase):
    def test_summary_is_zero_initialized_before_terminal_validation(self):
        initial = SOURCE.rsplit("static int initialize_summary(", 1)[1].split(
            "\n\nstatic void cleanup_summary(", 1)[0]
        self.assertIn("(*s)->nfilt = 0;", initial)
        self.assertIn("(*s)->nitems = 0;", initial)
        self.assertIn("(*s)->term_ncol = -1;", initial)
        self.assertIn("(*s)->term_nrow = -1;", initial)
        self.assertLess(
            initial.index("(*s)->nfilt = 0;"),
            initial.index("reconfigure_for_term_if_necessary(false, p, *s)"))
        self.assertIn("if (reconfigure_for_term_if_necessary(false, p, *s))", initial)
        self.assertIn("return 1;", initial)
        self.assertIn("filter_items(*s);", initial)
        self.assertIn("sort_items(*s);", initial)

    def test_bail_state_does_not_read_uninitialized_summary(self):
        block = SOURCE.split("void equip_cmp_display(void)", 1)[1].split(
            "\n\nstatic void display_equip_cmp_help(", 1)[0]
        self.assertIn("if (initialize_summary(player, &the_summary))", block)
        self.assertIn("istate = EQUIP_CMP_MENU_BAIL;", block)
        self.assertIn("angdroid_equip_cmp_summary_readable(", block)
        self.assertIn("istate == EQUIP_CMP_MENU_BAIL, the_summary != NULL", block)
        self.assertEqual(2, block.count("angdroid_equip_cmp_summary_readable("))
        self.assertIn("the_summary->dlg_trans_msg != NULL", block)
        self.assertIn("the_summary->nfilt == 0", block)
        self.assertIn("states[istate].prompt, hgt - 1, 0", block)
        self.assertIn("istate = (*states[istate].inputfunc)(in, istate, the_summary,", block)
        self.assertIn("screen_save();", block)
        self.assertIn("screen_load();", block)

    def test_no_unsafe_coordinates_or_cjk_partial_clipping(self):
        block = SOURCE.split(
            "static void equip_cmp_print_label(", 1)[1].split(
            "\n\nvoid equip_cmp_display(", 1)[0]
        self.assertIn('#include "equip-compare-state-guard.h"', SOURCE)
        self.assertIn("if (!Term || !angdroid_equip_cmp_label_region_valid(", block)
        self.assertIn("Term->wid, Term->hgt, row, col", block)
        self.assertIn("width = (size_t)(Term->wid - col);", block)
        self.assertIn("angdroid_menu_copy_columns(clipped, sizeof(clipped)", block)
        self.assertIn("prt(clipped, row, col);", block)
        self.assertIn("row >= 0 && row < height", HELPER)
        self.assertIn("col >= 0 && col < width", HELPER)
        self.assertIn("return !layout_failed && summary_exists;", HELPER)

    def test_previous_selection_and_export_contracts_are_unchanged(self):
        self.assertIn("display_page(the_summary, player, true)", SOURCE)
        self.assertEqual(2, SOURCE.count("display_page(s_for_file, player, false)"))
        self.assertIn("return strcmp(left->short_name, right->short_name);", SOURCE)
        self.assertIn("if (allow_reconfig && angdroid_zh_tw_enabled()", SOURCE)
        self.assertIn('file_putf(fff, "%s\\n", buf);', SOURCE)
        self.assertIn("tests/native/test_equip_cmp_tiny_terminal.c", RUNNER)
        self.assertIn("region(20, 5, 0, 20, false)", NATIVE)
        self.assertIn("summary(true, true, false)", NATIVE)


if __name__ == "__main__":
    unittest.main()
