#!/usr/bin/env python3
"""Full spell-reader source contract: no casting-key regression or CJK truncation."""
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SPELL = (ROOT / "app/src/main/cpp/angband/ui-spell.c").read_text(
    encoding="utf-8")
MENU = (ROOT / "app/src/main/cpp/angband/ui-menu.c").read_text(
    encoding="utf-8")
PAGES = (ROOT / "app/src/main/cpp/angband/zh-tw-spell-reader-pages.h").read_text(
    encoding="utf-8")
NATIVE = (ROOT / "tests/native/test_spell_reader_pages.c").read_text(
    encoding="utf-8")
RUNNER = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")


class FullSpellReaderSourceTest(unittest.TestCase):
    def test_separate_command_preserves_selection_and_casting(self):
        self.assertIn('m->cmd_keys = "?]";', SPELL)
        self.assertIn("if (e->key.code == ']')", SPELL)
        self.assertIn("spell_menu_show_full_description(spell);", SPELL)
        self.assertIn("d->selected_spell = d->spells[oid];", SPELL)
        self.assertIn("return d->browse ? true : false;", SPELL)
        self.assertIn("m->selections = all_letters_nohjkl;", SPELL)
        self.assertIn("menu_select(m, 0, true);", SPELL)

    def test_scroll_reader_restores_screen_and_handles_resizing(self):
        self.assertIn("screen_save();", SPELL)
        self.assertIn("screen_load();", SPELL)
        self.assertIn("if (width < 4 || height < 4) break;", SPELL)
        self.assertIn("if (in.type == EVT_RESIZE) {", SPELL)
        self.assertIn("saved_offset = top_offset;", SPELL)
        self.assertIn("reflow = true;", SPELL)
        self.assertIn("if (reflow) {", SPELL)
        self.assertIn("angdroid_spell_reader_line_for_offset(description,", SPELL)
        self.assertIn("first = angdroid_spell_reader_clamp_scroll(first, total, visible);", SPELL)
        self.assertIn("angdroid_spell_reader_clamp_scroll(first, total, visible)", SPELL)
        self.assertIn("angdroid_spell_reader_page_down(first, total, visible)", SPELL)
        self.assertIn("angdroid_spell_reader_page_up(first, visible)", SPELL)
        self.assertIn("case KC_PGDOWN:", SPELL)
        self.assertIn("case KC_PGUP:", SPELL)
        self.assertIn("case ESCAPE:", SPELL)
        self.assertIn("if (in.type == EVT_MOUSE)", SPELL)
        self.assertIn("angdroid_spell_reader_tap_action(", SPELL)
        self.assertIn("ANGDROID_SPELL_READER_TAP_BACK", SPELL)
        self.assertIn("ANGDROID_SPELL_READER_TAP_UP", SPELL)
        self.assertIn("ANGDROID_SPELL_READER_TAP_DOWN", SPELL)
        self.assertIn("if (in.type == EVT_ESCAPE) break;", SPELL)

    def test_narrow_screen_title_footer_and_rotation_safe_pagination(self):
        self.assertIn("angdroid_utf8_clip_columns(title, columns);", SPELL)
        self.assertIn("angdroid_utf8_clip_columns(status, columns);", SPELL)
        self.assertIn("my_strcpy(title, name, sizeof(title));", SPELL)
        self.assertIn("if (row == first) top_offset = start;", SPELL)
        self.assertIn("ANGDROID_SPELL_READER_TAP_IGNORE", PAGES)
        self.assertIn("x >= width || y >= height", PAGES)
        self.assertIn("y == 0 || y == height - 1", PAGES)
        self.assertIn("offset < next", PAGES)
        self.assertIn("angdroid_spell_reader_line_for_offset", NATIVE)
        self.assertIn("angdroid_spell_reader_tap_action", NATIVE)

    def test_original_source_and_existing_spell_browser_unchanged(self):
        self.assertIn('angdroid_zh_tw_spell_description(spell->name,', SPELL)
        self.assertIn("spell->text, zh);", SPELL)
        self.assertIn("text_out_hook = text_out_to_screen;", SPELL)
        self.assertIn('text_out("\\n%s", angdroid_zh_tw_spell_description(', SPELL)
        self.assertIn("angdroid_utf8_next_scalar", PAGES)
        self.assertIn("angdroid_terminal_columns", PAGES)
        self.assertIn("memcpy(out, text + start, end - start);", PAGES)
        self.assertIn("out[end - start] = '\\0';", PAGES)

    def test_modal_exit_and_parent_menu_relayout(self):
        # The nested reader consumes EVT_RESIZE: the parent menu_select()
        # never sees it, so a fresh menu_layout() is required upon return.
        self.assertIn("static void spell_menu_relayout(struct menu *m)", SPELL)
        self.assertIn("Term_get_size(&width, &height);", SPELL)
        self.assertIn("menu_width = angdroid_spell_reader_menu_width(width);", SPELL)
        self.assertIn("bounds.col = -menu_width;", SPELL)
        self.assertIn("bounds.page_rows = m->count + 1;", SPELL)
        self.assertIn("(void)menu_layout(m, &bounds);", SPELL)
        self.assertIn("spell_menu_show_full_description(spell);", SPELL)
        self.assertIn("spell_menu_relayout(m);", SPELL)
        self.assertEqual(2, SPELL.count("spell_menu_relayout(m);"))
        self.assertIn("angdroid_spell_reader_menu_width", PAGES)
        self.assertIn("terminal_width <= 15", PAGES)
        self.assertIn("terminal_width - 15 < 80", PAGES)
        self.assertIn("angdroid_spell_reader_menu_width(95) == 80", NATIVE)
        self.assertIn("angdroid_spell_reader_menu_width(20) == 5", NATIVE)

        # A single screen-save/screen-load pair for every reader path.
        modal = SPELL.split("static void spell_menu_show_full_description", 1)[1]
        modal = modal.split("static void spell_menu_relayout", 1)[0]
        self.assertEqual(1, modal.count("screen_save();"))
        self.assertEqual(1, modal.count("screen_load();"))
        self.assertIn("while (!finished)", modal)
        self.assertIn("finished = true;", modal)
        self.assertNotIn("return;", modal.split("screen_save();", 1)[1])
        self.assertNotIn("m->cursor =", modal)

    def test_spell_row_taps_select_exactly_once(self):
        # Active row mouse taps are delivered as the original EVT_MOUSE,
        # rather than the translated EVT_SELECT, after menu cursor moves.
        dispatch = MENU.split("ui_event menu_select(", 1)[1]
        self.assertIn("menu_handle_mouse(menu, &in, &out);", dispatch)
        self.assertIn("if (out.type == EVT_SELECT && !no_act)", dispatch)
        self.assertIn("touch_select_dispatched = true;", dispatch)
        self.assertIn("menu_handle_action(menu, &in)", dispatch)
        self.assertIn("!touch_select_dispatched &&", dispatch)
        self.assertIn("menu_handle_action(menu, &out)", dispatch)

        handler = SPELL.split("static bool spell_menu_handler(", 1)[1]
        handler = handler.split("static void spell_menu_browser(", 1)[0]
        self.assertIn("if (e->type == EVT_SELECT ||", handler)
        self.assertIn("(e->type == EVT_MOUSE &&", handler)
        self.assertIn("angdroid_spell_menu_touch_selectable(", handler)
        self.assertIn("region_inside(&m->active, e)", handler)
        self.assertIn("e->mouse.button, oid)", handler)
        self.assertIn("if (oid < 0 || oid >= d->n_spells) return false;", handler)
        self.assertIn("d->selected_spell = d->spells[oid];", handler)
        self.assertIn("return d->browse ? true : false;", handler)
        self.assertEqual(1, handler.count("d->selected_spell = d->spells[oid];"))

        # An out-of-region header/back event must not cast the previously
        # selected row. Test the same eligibility rule in portable native C.
        self.assertIn("angdroid_spell_menu_touch_selectable(", PAGES)
        self.assertIn("inside_active && button != 2 && oid >= 0", PAGES)
        self.assertIn("angdroid_spell_menu_touch_selectable(1, 0, 6)", NATIVE)
        self.assertIn("!angdroid_spell_menu_touch_selectable(0, 1, 4)", NATIVE)
        self.assertIn("!angdroid_spell_menu_touch_selectable(1, 2, 5)", NATIVE)
        self.assertIn("!angdroid_spell_menu_touch_selectable(1, 1, -1)", NATIVE)

    def test_native_cases_are_registered(self):
        for phrase in ("A中文B", "Mana Storm hits.", "Alpha  Beta",
                       "甲\\n\\n乙", "angdroid_spell_reader_page_down",
                       "angdroid_spell_reader_page_up"):
            with self.subTest(phrase=phrase):
                self.assertIn(phrase, NATIVE)
        self.assertIn('test_spell_reader_pages.c', RUNNER)
        self.assertIn('tests/test_spell_reader_source.py', VERIFY)


if __name__ == "__main__":
    unittest.main()
