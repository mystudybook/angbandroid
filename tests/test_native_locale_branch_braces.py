#!/usr/bin/env python3
"""Regressions for Android NDK -Wdangling-else locale-message warnings.

Check that the outer gameplay condition owns BOTH translated branches.
The C compiler remains the authority: this static check is run alongside
the optional Gradle/NDK compile, not in place of it.
"""
from pathlib import Path
import re
import unittest

ROOT = Path(__file__).resolve().parents[1] / "app/src/main/cpp/angband"


class NativeLocaleBranchBracesTest(unittest.TestCase):
    def assert_scoped_locale_choice(self, filename, outer, chinese, english):
        source = (ROOT / filename).read_text(encoding="utf-8")
        pattern = re.compile(
            r"if\s*\(\s*" + re.escape(outer) +
            r"\s*\)\s*\{\s*if\s*\(\s*angdroid_zh_tw_enabled\(\)\s*\)\s*" +
            r"(?P<zh>msgt?\s*\([^;]*;)\s*else\s*" +
            r"(?P<en>msgt?\s*\([^;]*;)\s*\}",
            re.S,
        )
        matches = list(pattern.finditer(source))
        self.assertTrue(
            any(chinese in match.group("zh") and english in match.group("en")
                for match in matches),
            f"{filename}: locale branches for {outer!r} must be inside braces",
        )

    def test_out_of_depth_notifications(self):
        for chinese, english in (
            ("深層獨特怪物", "Deep unique"),
            ("深層怪物", "Deep monster"),
        ):
            with self.subTest(chinese=chinese):
                self.assert_scoped_locale_choice(
                    "mon-make.c", "OPT(player, cheat_hear)", chinese, english
                )

    def test_mimic_reveal_only_if_visible(self):
        self.assert_scoped_locale_choice(
            "mon-util.c", "square_isseen(c, obj->grid)",
            "原來%s是一隻怪物", "The %s was really a monster",
        )

    def test_monster_push_only_if_visible(self):
        self.assert_scoped_locale_choice(
            "mon-move.c",
            "monster_is_visible(mon) && monster_is_in_view(mon)",
            "%s%s%s。", "%s %s %s.",
        )

    def test_blind_summoning_message_only_for_blind_player(self):
        self.assert_scoped_locale_choice(
            "effect-handler-general.c", "count && player->timed[TMD_BLIND]",
            "你聽見附近出現了%s", "You hear %s appear nearby",
        )


if __name__ == "__main__":
    unittest.main()
