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
        self.assertIn("if (e->type == EVT_MOUSE) {", handler)
        self.assertIn("angdroid_spell_menu_touch_selectable(", handler)
        self.assertIn("region_inside(&m->active, e)", handler)
        self.assertIn("e->mouse.button, oid)", handler)
        self.assertIn("oid >= d->n_spells", handler)
        self.assertIn("angdroid_spell_menu_confirm_touch(", handler)
        self.assertIn("&d->touch_pending_oid, &d->touch_pending_at_ms,", handler)
        self.assertIn("oid, d->browse, spell_menu_monotonic_ms()", handler)
        self.assertIn("else if (e->type == EVT_SELECT) {", handler)
        self.assertIn("else if (e->type == EVT_KBRD) {", handler)
        self.assertIn("angdroid_spell_menu_cancel_touch(&d->touch_pending_oid,", handler)
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

    def test_two_taps_required_for_casting_and_studying(self):
        self.assertIn("static inline int angdroid_spell_menu_confirm_touch(", PAGES)
        self.assertIn("static inline void angdroid_spell_menu_cancel_touch(", PAGES)
        self.assertIn("ANGDROID_SPELL_TOUCH_CONFIRM_WINDOW_MS UINT64_C(2000)", PAGES)
        self.assertIn("uint64_t *armed_at_ms, int oid, int browse,", PAGES)
        self.assertIn("uint64_t now_ms)", PAGES)
        self.assertIn("if (browse) {", PAGES)
        self.assertIn("*armed_oid == oid && *armed_at_ms != 0 && now_ms != 0", PAGES)
        self.assertIn("now_ms >= *armed_at_ms", PAGES)
        self.assertIn("now_ms - *armed_at_ms <=", PAGES)
        self.assertIn("ANGDROID_SPELL_TOUCH_CONFIRM_WINDOW_MS", PAGES)
        self.assertIn("*armed_oid = oid;", PAGES)
        self.assertIn("*armed_at_ms = now_ms;", PAGES)
        self.assertIn("if (!armed_oid || !armed_at_ms) return 0;", PAGES)
        self.assertIn("angdroid_spell_menu_cancel_touch(armed_oid, armed_at_ms);",
                      PAGES)

        handler = SPELL.split("static bool spell_menu_handler(", 1)[1]
        handler = handler.split("static void spell_menu_browser(", 1)[0]
        self.assertIn("if (!angdroid_spell_menu_confirm_touch(", handler)
        self.assertIn("&d->touch_pending_oid, &d->touch_pending_at_ms,", handler)
        self.assertIn("oid, d->browse, spell_menu_monotonic_ms()", handler)
        self.assertIn("return true;", handler)
        self.assertEqual(1, handler.count("d->selected_spell = d->spells[oid];"))
        self.assertIn("uint64_t touch_pending_at_ms;", SPELL)
        self.assertIn("angdroid_spell_menu_cancel_touch(&d->touch_pending_oid,",
                      SPELL)
        self.assertIn("if (d->touch_pending_oid >= 0 && d->touch_pending_oid != oid)",
                      SPELL)
        self.assertIn("spell_menu_relayout\t/* reflow menu", SPELL)
        self.assertIn("d->browse = true;", SPELL)
        self.assertIn("m->selections = all_letters_nohjkl;", SPELL)
        self.assertNotIn("m->flags = MN_CASELESS_TAGS | MN_DBL_TAP", SPELL)
        self.assertIn("2 秒內觸控同列兩次確認", SPELL)
        self.assertIn("Tap same row twice within 2s", SPELL)
        self.assertIn("char buf[192];", SPELL)
        self.assertIn("angdroid_utf8_clip_columns(buf, (size_t)(Term->wid - 1));",
                      SPELL)

        # C99 must use a monotonic millisecond clock, not time(NULL),
        # clock() (process CPU time), or adjustable gettimeofday().
        self.assertIn("#define _POSIX_C_SOURCE 200809L", SPELL)
        self.assertIn("#include <time.h>", SPELL)
        self.assertIn("clock_gettime(CLOCK_MONOTONIC, &ts)", SPELL)
        self.assertIn("(uint64_t)ts.tv_nsec / UINT64_C(1000000)", SPELL)
        self.assertIn("if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0", SPELL)
        self.assertNotIn("gettimeofday(", SPELL)
        self.assertNotIn("time(NULL)", SPELL)
        BUILD = (ROOT / "app/build.gradle").read_text(encoding="utf-8")
        self.assertIn("minSdk = 22", BUILD)

        # The Android terminal delivers a single mouse press after
        # ACTION_UP; keyboard Enter and spell-letter tags remain unchanged.
        JAVA = (ROOT / "app/src/main/java/org/rephial/xyangband/TermView.java").read_text(
            encoding="utf-8")
        self.assertIn("public boolean onSingleTapUp(MotionEvent event)", JAVA)
        self.assertIn("sendMousePress(y, x);", JAVA)
        self.assertIn("state.addMousePress(p.y, p.x, 1, 0);", JAVA)

        for phrase in (
            "ANGDROID_SPELL_TOUCH_CONFIRM_WINDOW_MS == UINT64_C(2000)",
            "UINT64_C(3000)",
            "UINT64_C(6001)",
            "UINT64_C(4999)",
            "UINT64_MAX",
            "angdroid_spell_menu_cancel_touch(&armed, &at)",
            "angdroid_spell_menu_confirm_touch(",
        ):
            with self.subTest(phrase=phrase):
                self.assertIn(phrase, NATIVE)

    def test_timeout_does_not_lose_existing_keyboard_behavior(self):
        handler = SPELL.split("static bool spell_menu_handler(", 1)[1]
        handler = handler.split("static void spell_menu_browser(", 1)[0]
        self.assertIn("else if (e->type == EVT_SELECT) {", handler)
        self.assertIn("else if (e->type == EVT_KBRD) {", handler)
        self.assertIn("d->selected_spell = d->spells[oid];", handler)
        self.assertIn("return d->browse ? true : false;", handler)
        self.assertIn("if (e->key.code == ']')", handler)
        self.assertIn("if (e->key.code == '?')", handler)
        self.assertIn("spell_menu_relayout(m);", handler)
        self.assertIn("angdroid_spell_menu_cancel_touch(&d->touch_pending_oid,",
                      handler)
        self.assertEqual(1, handler.count("d->selected_spell = d->spells[oid];"))

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
