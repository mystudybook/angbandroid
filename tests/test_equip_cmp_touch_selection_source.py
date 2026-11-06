#!/usr/bin/env python3
"""Source regression for equipment comparison touch/resize and zh-TW prompts.

The selected filtered index must never be used to access sorted_indices
unless the current page and index are valid. No game-data IDs are translated.
"""
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp/angband"
SOURCE = (CPP / "ui-equip-cmp.c").read_text(encoding="utf-8")
GUARD = (CPP / "equip-compare-touch-guard.h").read_text(encoding="utf-8")
LABELS = (CPP / "zh-tw-equip-cmp.h").read_text(encoding="utf-8")
NATIVE = (ROOT / "tests/native/test_equip_cmp_touch_guard.c").read_text(
    encoding="utf-8")
NATIVE_RUNNER = (ROOT / "scripts/test-native-utf8.sh").read_text(
    encoding="utf-8")


class EquipmentCompareTouchSelectionTest(unittest.TestCase):
    def test_empty_filtered_list_cannot_enter_select(self):
        general = SOURCE.rsplit(
            "static int handle_input_equip_cmp_general(", 1)[1].split(
            "\n\nstatic void display_equip_cmp_sel_help(", 1)[0]
        self.assertIn('static const char *trans_msg_no_items = "No items to examine";', general)
        self.assertIn("case ACT_CTX_EQUIPCMP_START_SELECT:", general)
        self.assertIn(
            "!angdroid_equip_cmp_selection_valid(s->ifirst, s->nfilt)", general)
        self.assertIn("s->npage <= 0", general)
        self.assertIn("s->dlg_trans_msg = trans_msg_no_items;", general)
        self.assertIn("result = EQUIP_CMP_MENU_NEW_PAGE;", general)
        self.assertIn("s->work_sel = s->ifirst;", general)

    def test_touch_row_has_both_coordinate_and_index_guards(self):
        select = SOURCE.rsplit(
            "static int handle_input_equip_cmp_select(", 1)[1].split(
            "\n\nstatic int prompt_for_easy_filter(", 1)[0]
        self.assertIn("if (Term && angdroid_equip_cmp_touch_index(", select)
        self.assertIn("in.mouse.x, in.mouse.y, Term->wid, Term->hgt", select)
        self.assertIn("s->ifirst, s->indinc, s->nfilt, &new_sel", select)
        self.assertIn("if (s->work_sel == new_sel)", select)
        self.assertIn("action = ACT_CTX_EQUIPCMP_SELECT_ACCEPT;", select)
        self.assertIn(
            "angdroid_equip_cmp_label_region_valid(", select)
        self.assertIn("menu_dynamic_select(m)", select)
        self.assertIn("menu_dynamic_free(m)", select)
        self.assertIn("case ACT_CTX_EQUIPCMP_SELECT_ACCEPT:", select)
        self.assertIn(
            "!angdroid_equip_cmp_selection_valid(s->work_sel, s->nfilt)",
            select)
        self.assertIn("s->sorted_indices[s->work_sel]", select)
        self.assertNotIn(
            "assert(s->work_sel >= 0 && s->work_sel < s->nfilt)", select)
        self.assertIn("case ACT_CTX_EQUIPCMP_SELECT_ESCAPE:", select)
        self.assertIn("case ACT_CTX_EQUIPCMP_SELECT_SKIP:", select)

    def test_resize_clamps_filtered_selection_without_changing_ids(self):
        resize = SOURCE.rsplit(
            "static int reconfigure_for_term_if_necessary(", 1)[1].split(
            "\n\nstatic void compute_player_and_equipment_values(", 1)[0]
        self.assertIn(
            "angdroid_equip_cmp_clamp_visible_selection(", resize)
        self.assertIn("update_names && !result && s->work_sel != -1", resize)
        self.assertIn("s->ifirst, s->npage, s->indinc, s->nfilt", resize)
        self.assertIn("set_short_name(s->items[i].obj, length, p);", resize)
        self.assertIn("return result;", resize)
        self.assertIn("return strcmp(left->short_name, right->short_name);", SOURCE)
        self.assertEqual(2, SOURCE.count("display_page(s_for_file, player, false)"))
        self.assertIn("display_page(the_summary, player, true)", SOURCE)

    def test_bilingual_touch_prompts_and_unchanged_actions(self):
        display = SOURCE.split("void equip_cmp_display(void)", 1)[1].split(
            "\n\nstatic void display_equip_cmp_help(", 1)[0]
        self.assertIn("Tap a row to select; tap again to confirm", display)
        self.assertIn("istate == EQUIP_CMP_MENU_SEL0", display)
        self.assertIn("istate == EQUIP_CMP_MENU_SEL1", display)
        self.assertIn(
            '{ "Tap a row to select; tap again to confirm", '
            '"點選物品列以選取，再點一次確認" }', LABELS)
        self.assertIn(
            '{ "No items to examine", "沒有可查看的裝備。" }', LABELS)
        self.assertIn("case KC_ENTER:", SOURCE)
        self.assertIn("case ESCAPE:", SOURCE)
        self.assertIn("case 'x':", SOURCE)
        self.assertIn("if (in.mouse.button == 2)", SOURCE)

    def test_header_math_and_runner(self):
        self.assertIn("int64_t index;", GUARD)
        self.assertIn("int64_t last, offset;", GUARD)
        self.assertIn("x < 0 || x >= width", GUARD)
        self.assertIn("y < 0 || y >= height", GUARD)
        self.assertIn("direction != 1 && direction != -1", GUARD)
        self.assertIn("index < 0 || index >= filtered_count", GUARD)
        self.assertIn("if (!selected) return false;", GUARD)
        self.assertIn("if (selected == -1) return -1;", GUARD)
        self.assertIn("touch(0, 8, 80, 24, 4, 5, 18, 1, 20, false, -1)", NATIVE)
        self.assertIn("clamp(7, 8, 4, -1, 10, 7)", NATIVE)
        self.assertIn("tests/native/test_equip_cmp_touch_guard.c", NATIVE_RUNNER)


if __name__ == "__main__":
    unittest.main()
