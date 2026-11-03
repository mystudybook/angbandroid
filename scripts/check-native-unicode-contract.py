#!/usr/bin/env python3
"""Non-compiling sanity checks for the Android Angband native CJK text path.

This checks wiring/invariants only. It CANNOT establish correct rendering,
native compilation, game layout or Android-device behavior.
"""
from pathlib import Path
import sys


ROOT = Path(__file__).resolve().parents[1]


def main():
    source_checks = {
        "app/src/main/cpp/angband/ui-term.c": {
            'uses_width_fit': "angdroid_fit_terminal_text(s, (size_t)count,",
            'raw_tile_path_preserved': "static void Term_queue_char_raw(",
            'tracks_native_cursor_columns': "Term->scr->cx += span;",
            'queue_wide_marker': "Term_queue_char_raw(t, x + 1, y, a,",
            'erase_half_pair': "Term_clear_wide_neighbor(t, x, y);",
            'erase_span_policy': "Removing either half of a two-column glyph",
            'partial_redraw_expansion': "angdroid_expand_redraw_columns(Term->wid, &first, &last,",
            'redraw_range_is_row_local': "int first = x1;",
            'partner_cell_on_overwrite': "angdroid_wide_partner_column(t->wid, x,",
            'merge_existing_dirty_ranges': "angdroid_union_dirty_range(first, last,",
        },
        "app/src/main/cpp/curses/unicode_layout.h": {
            'redraw_helper': "static inline void angdroid_expand_redraw_columns(",
            'wide_partner_helper': "static inline int angdroid_wide_partner_column(",
            'dirty_range_union_helper': "static inline void angdroid_union_dirty_range(",
            'preserves_large_tile': "previous_attrs[*first] == 255",
        },
        "app/src/main/cpp/common/angdroid.c": {
            'unicode_text_runs': "angdroid_next_visible_text_run(",
            'text_hook_uses_native_offsets': "wmove(td->win, y, x + (int)start);",
            'graphics_mode_preserves_unicode': "Term_text_android(x + i, y, 1, a, &cp[i]);",
            'utf8_decode_hook': "text_mbcs_hook = angdroid_decode_utf8_wide;",
            'utf8_encode_hook': "text_wctomb_hook = android_utf8_encode_character;",
        },
        "app/src/main/cpp/curses/curses.c": {
            'locale_independent_encoder': "angdroid_encode_wide_utf8(ws, (size_t)n, utf8, capacity)",
        },
        "app/src/main/java/org/rephial/xyangband/TermWindow.java": {
            'continuation_cells': "public boolean isTextContinuation()",
            'wide_cursor': "if (width == 2) advance();",
            'explicit_jni_length': "new String(cp, 0, byteCount, encoding)",
            'jni_length_clamp': "Math.min(n, cp.length)",
            'clear_bottom_row_origin': "int first = (r == row) ? col : 0;",
        },
        "app/src/main/java/org/rephial/xyangband/TermView.java": {
            'two_cell_clip': "int tw = char_width * (p.textColumns == 2 ? 2 : 1);",
        },
        "scripts/test-native-utf8.sh": {
            'native_layout_test': "test_unicode_layout.c",
            'utf8_test': "test_unicode_utf8.c",
        },
    }

    failures = []
    passed = 0
    for filename, checks in source_checks.items():
        path = ROOT / filename
        try:
            content = path.read_text(encoding="utf-8")
        except OSError as exc:
            failures.append(f"{filename}: {exc}")
            continue
        for label, pattern in checks.items():
            if pattern in content:
                passed += 1
            else:
                failures.append(f"{filename}: missing {label} [{pattern}]")

    if failures:
        for message in failures:
            print("FAIL:", message)
        return 1
    print(f"PASS: {passed} native/Java Unicode protocol wiring checks")
    print("NOTE: Static checks only; requires Android build and device testing")
    return 0


if __name__ == "__main__":
    sys.exit(main())
