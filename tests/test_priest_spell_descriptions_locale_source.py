#!/usr/bin/env python3
"""Data-driven Priest spell descriptions, with Paladin Heroism mismatch safety."""
import importlib.util
import json
import re
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SPEC = importlib.util.spec_from_file_location(
    "angband_mage_spell_descriptions",
    ROOT / "tests/test_spell_descriptions_locale_source.py")
assert SPEC is not None and SPEC.loader is not None
MODULE = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(MODULE)
SPELLS = MODULE.parsed_spells()
MAPPED = MODULE.translated_entries(MODULE.HELPER)
PRIEST = {(x["name"], x["english"]) for x in SPELLS if x["class"] == "Priest"}
MAGE = {(x["name"], x["english"]) for x in SPELLS if x["class"] == "Mage"}
DRUID = {(x["name"], x["english"]) for x in SPELLS if x["class"] == "Druid"}
PALADIN = {x["name"]: x["english"] for x in SPELLS if x["class"] == "Paladin"}
NATIVE = (ROOT / "tests/native/test_priest_spell_descriptions_locale.c").read_text(
    encoding="utf-8")
RUNNER = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")


class PriestSpellDescriptionsLocaleSourceTest(unittest.TestCase):
    def test_canonical_priest_coverage_and_preserved_mage_druid(self):
        self.assertEqual(163, len(SPELLS))
        self.assertEqual(30, len(MAGE))
        self.assertEqual(27, len(DRUID))
        self.assertEqual(28, len(PRIEST))
        self.assertEqual(85, len(MAPPED))
        self.assertEqual(85, len({(n, en) for n, en, zh in MAPPED}))
        self.assertEqual(85, len({name for name, en, zh in MAPPED}))
        self.assertEqual(MAGE | DRUID | PRIEST,
                         {(n, en) for n, en, zh in MAPPED})
        for n, en, zh in MAPPED:
            with self.subTest(spell=n):
                self.assertTrue(en.strip() and zh.strip())
                self.assertNotEqual(en, zh)
                self.assertNotIn("\n", zh)
                self.assertNotIn("\r", zh)

    def test_paladin_14_identical_and_heroism_difference(self):
        priest = dict(PRIEST)
        shared = set(priest) & set(PALADIN)
        same = {name for name in shared if priest[name] == PALADIN[name]}
        self.assertEqual(15, len(shared))
        self.assertEqual(14, len(same))
        self.assertEqual({"Heroism"}, shared - same)
        self.assertIn("character level 20", priest["Heroism"])
        self.assertIn("character level 15", PALADIN["Heroism"])
        self.assertNotIn(("Heroism", PALADIN["Heroism"]),
                         {(n, en) for n, en, zh in MAPPED})

    def test_prayer_warnings_and_numeric_values(self):
        zh = {n: text for n, en, text in MAPPED if (n, en) in PRIEST}
        for name, token in (
            ("Minor Healing", "20"),
            ("Bless", "5"),
            ("Bless", "10"),
            ("Sense Invisible", "24+1d24"),
            ("Heroism", "20"),
            ("Heroism", "12"),
            ("Orb of Draining", "30"),
            ("Orb of Draining", "雙倍"),
            ("Word of Recall", "14+1d20"),
            ("Word of Recall", "取消"),
            ("Word of Recall", "限制樓梯"),
            ("Word of Recall", "魔苟斯"),
            ("Healing", "2000"),
            ("Word of Destruction", "15"),
            ("Word of Destruction", "10+1d10"),
            ("Word of Destruction", "城鎮"),
            ("Word of Destruction", "樓梯"),
            ("Holy Word", "1000"),
            ("Light of Manwë", "4"),
            ("Light of Manwë", "100"),
            ("Glyph of Warding", "破壞"),
        ):
            with self.subTest(name=name, token=token):
                self.assertIn(token, zh[name])

    def test_native_rows_match_canonical_and_test_scripts(self):
        pattern = re.compile(
            r'^\s*check\(("(?:[^"\\]|\\.)*"),\s*'
            r'("(?:[^"\\]|\\.)*"),\s*'
            r'("(?:[^"\\]|\\.)*")\);$', re.M)
        rows = [tuple(json.loads(s) for s in match.groups())
                for match in pattern.finditer(NATIVE)]
        expected = {(n, en, zh) for n, en, zh in MAPPED
                    if (n, en) in PRIEST}
        self.assertEqual(28, len(rows))
        self.assertEqual(expected, set(rows))
        self.assertIn('paladin_heroism, 1) == paladin_heroism', NATIVE)
        self.assertIn("tests/native/test_priest_spell_descriptions_locale.c",
                      RUNNER)
        self.assertIn("tests/test_priest_spell_descriptions_locale_source.py",
                      VERIFY)


if __name__ == "__main__":
    unittest.main()
