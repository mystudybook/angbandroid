#!/usr/bin/env python3
"""Exercise current Angband display contracts and their negative regressions.

The native localization checker used to insist on obsolete source strings
even after the safer popup, lore and rune renderers replaced them.  Evaluate
its four guard predicates against current files and deliberately broken
copies, so fixes to the checker cannot silently remove their safety intent.
"""
from __future__ import annotations

import ast
from pathlib import Path
import re
import unittest

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp/angband"
CHECKER = ROOT / "scripts/check_native_game_localization.py"

LABELS = {
    "menu": "Dynamic menu geometry needs a screen-width bound",
    "heading": "Knowledge browser heading lacks Chinese display text",
    "monster": "Monster message translation and Chinese subjects are not wired",
    "columns": "Knowledge item/rune entries must stop before metadata columns",
}


def get_guard(label: str):
    tree = ast.parse(CHECKER.read_text(encoding="utf-8"), str(CHECKER))
    main = [n for n in tree.body if isinstance(n, ast.FunctionDef)
            and n.name == "main"]
    if len(main) != 1:
        raise AssertionError("Expected a single native checker main()")
    checks = [
        call.args[0]
        for call in ast.walk(main[0])
        if isinstance(call, ast.Call)
        and isinstance(call.func, ast.Name) and call.func.id == "expect"
        and len(call.args) >= 2
        and isinstance(call.args[1], ast.Constant)
        and call.args[1].value == label
    ]
    if len(checks) != 1:
        raise AssertionError(f"Expected exactly one guard for {label}")
    return compile(ast.Expression(body=checks[0]), str(CHECKER), "eval")


class CurrentDisplayContractsTest(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.sources = {
            "menu_ui": (CPP / "ui-menu.c").read_text(encoding="utf-8"),
            "knowledge_ui": (CPP / "ui-knowledge.c").read_text(encoding="utf-8"),
            "mon_msg_ui": (CPP / "mon-msg.c").read_text(encoding="utf-8"),
        }

    def guard_passes(self, label: str, **source_overrides):
        environment = dict(self.sources)
        environment.update(source_overrides)
        # The expression is obtained from a locally tracked Python checker.
        # Restricted globals are sufficient for these string/regex guards.
        return bool(eval(get_guard(label), {"__builtins__": {}, "re": re},
                         environment))

    def assert_mutation_rejected(self, label, source_name, old, replacement):
        original = self.sources[source_name]
        self.assertIn(old, original)
        self.assertTrue(self.guard_passes(label), label)
        mutated = original.replace(old, replacement, 1)
        self.assertFalse(self.guard_passes(label, **{source_name: mutated}),
                         f"Unprotected {label!r} mutation passed")

    def test_dynamic_popup_stays_bounded_to_screen(self):
        self.assert_mutation_rejected(
            LABELS["menu"], "menu_ui",
            "Term->wid, Term->hgt, mx, my, menu_dynamic_longest_entry(m),",
            "120, 40, mx, my, menu_dynamic_longest_entry(m),",
        )
        self.assert_mutation_rejected(
            LABELS["menu"], "menu_ui",
            "r.width = popup.width;", "r.width = 500;",
        )

    def test_knowledge_title_uses_translated_display_name(self):
        self.assert_mutation_rejected(
            LABELS["heading"], "knowledge_ui",
            'format("知識圖鑑：%s", display_title)',
            'format("知識圖鑑：%s", title)',
        )

    def test_monster_message_keeps_race_and_subject_flags(self):
        self.assert_mutation_rejected(
            LABELS["monster"], "mon_msg_ui",
            "get_monster_message_zh_tw(msg->msg_code, msg->race)",
            "get_monster_message_zh_tw(msg->msg_code)",
        )
        self.assert_mutation_rejected(
            LABELS["monster"], "mon_msg_ui",
            "(msg->flags & MON_MSG_FLAG_OFFSCREEN) != 0",
            "false",
        )

    def test_runes_translate_before_metadata_column_clipping(self):
        self.assert_mutation_rejected(
            LABELS["columns"], "knowledge_ui",
            "display_name, col, 46, Term->wid",
            "display_name, col, Term->wid, Term->wid",
        )
        self.assert_mutation_rejected(
            LABELS["columns"], "knowledge_ui",
            "angdroid_zh_tw_rune_title(", "rune_name_without_localization(",
        )


if __name__ == "__main__":
    unittest.main()
