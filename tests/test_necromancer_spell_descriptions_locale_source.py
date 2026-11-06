#!/usr/bin/env python3
"""Original Necromancer descriptions: canonical text and adverse-effect safety."""
from pathlib import Path
import importlib.util
import json
import re
import unittest

ROOT = Path(__file__).resolve().parents[1]
SPEC = importlib.util.spec_from_file_location(
    "angband_spell_description_data",
    ROOT / "tests/test_spell_descriptions_locale_source.py")
assert SPEC is not None and SPEC.loader is not None
SOURCE = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(SOURCE)
SPELLS = SOURCE.parsed_spells()
MAPPED = SOURCE.translated_entries(SOURCE.HELPER)
NECRO = {(x["name"], x["english"]) for x in SPELLS
         if x["class"] == "Necromancer"}
PRIEST = {(x["name"], x["english"]) for x in SPELLS
          if x["class"] == "Priest"}
MAGE = {(x["name"], x["english"]) for x in SPELLS if x["class"] == "Mage"}
DRUID = {(x["name"], x["english"]) for x in SPELLS if x["class"] == "Druid"}
TEST = (ROOT / "tests/native/test_necromancer_spell_descriptions_locale.c").read_text(
    encoding="utf-8")
RUNNER = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")


class NecromancerDescriptionLocaleSourceTest(unittest.TestCase):
    def test_complete_canonical_and_reuse_without_duplicates(self):
        self.assertEqual(163, len(SPELLS))
        self.assertEqual(26, len(NECRO))
        self.assertEqual(110, len(MAPPED))
        self.assertEqual(110, len({(n, en) for n, en, zh in MAPPED}))
        self.assertEqual(110, len({n for n, en, zh in MAPPED}))
        self.assertEqual(MAGE | DRUID | PRIEST | NECRO,
                         {(n, en) for n, en, zh in MAPPED})
        self.assertEqual({("Sense Invisible",
                           "Enables you to see invisible monsters for 24+1d24 turns.")},
                         NECRO & PRIEST)
        for name, en, zh in MAPPED:
            with self.subTest(name=name):
                self.assertTrue(en.strip() and zh.strip())
                self.assertNotEqual(en, zh)
                self.assertNotIn("\n", zh)
                self.assertNotIn("\r", zh)

    def test_self_damage_and_target_restrictions(self):
        text = {name: zh for name, en, zh in MAPPED
                if (name, en) in NECRO}
        for name, token in (
            ("Bat Form", "無法使用物品"),
            ("Crush", "四倍"),
            ("Crush", "玩家也會受到傷害"),
            ("Sleep Evil", "抵抗混亂"),
            ("Shadow Shift", "身體"),
            ("Warg Form", "生命值"),
            ("Annihilate", "四倍"),
            ("Annihilate", "魔像"),
            ("Grond's Blow", "半徑 15"),
            ("Grond's Blow", "10+1d10"),
            ("Grond's Blow", "抗黑暗"),
            ("Grond's Blow", "城鎮"),
            ("Unleash Chaos", "喚醒"),
            ("Storm of Darkness", "半徑為 4"),
            ("Power Sacrifice", "50 點生命值"),
            ("Power Sacrifice", "50 點法力"),
            ("Zone of Unmagic", "玩家自身"),
            ("Vampire Form", "目前生命值的一半"),
            ("Vampire Form", "撕咬"),
            ("Curse", "100 點生命值"),
            ("Command", "r 解除控制"),
        ):
            with self.subTest(name=name, token=token):
                self.assertIn(token, text[name])

    def test_native_rows_match_original_data_and_runner(self):
        matcher = re.compile(
            r'^\s*check\(("(?:[^"\\]|\\.)*"),\s*'
            r'("(?:[^"\\]|\\.)*"),\s*'
            r'("(?:[^"\\]|\\.)*")\);$', re.M)
        test_rows = [tuple(json.loads(piece) for piece in m.groups())
                     for m in matcher.finditer(TEST)]
        expected = {(n, en, zh) for n, en, zh in MAPPED if (n, en) in NECRO}
        self.assertEqual(26, len(test_rows))
        self.assertEqual(expected, set(test_rows))
        self.assertIn('test_necromancer_spell_descriptions_locale.c', RUNNER)
        self.assertIn('tests/test_necromancer_spell_descriptions_locale_source.py',
                      VERIFY)
        self.assertIn('new_description, 1) == new_description', TEST)


if __name__ == "__main__":
    unittest.main()
