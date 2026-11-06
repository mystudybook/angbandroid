#!/usr/bin/env python3
"""All original 163 class spell records translated with exact-source safety."""
import importlib.util
import json
import re
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SPEC = importlib.util.spec_from_file_location(
    "angband_spell_description_source",
    ROOT / "tests/test_spell_descriptions_locale_source.py")
assert SPEC is not None and SPEC.loader is not None
SOURCE = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(SOURCE)
SPELLS = SOURCE.parsed_spells()
MAPPED = SOURCE.translated_entries(SOURCE.HELPER)
BLACKGUARD = {(x["name"], x["english"]) for x in SPELLS
              if x["class"] == "Blackguard"}
NATIVE = (ROOT / "tests/native/test_blackguard_spell_descriptions_locale.c").read_text(
    encoding="utf-8")
RUNNER = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")


class BlackguardSpellDescriptionsLocaleSourceTest(unittest.TestCase):
    def test_complete_canonical_163_records(self):
        all_classes = {"Mage": 30, "Druid": 27, "Priest": 28,
                       "Necromancer": 26, "Paladin": 16, "Rogue": 10,
                       "Ranger": 11, "Blackguard": 15}
        self.assertEqual(163, len(SPELLS))
        for cls, count in all_classes.items():
            with self.subTest(cls=cls):
                self.assertEqual(count, sum(s["class"] == cls for s in SPELLS))
        self.assertEqual(set(all_classes), {s["class"] for s in SPELLS})
        self.assertEqual(15, len(BLACKGUARD))
        self.assertEqual(135, len(MAPPED))
        self.assertEqual(135, len({(name, en) for name, en, zh in MAPPED}))
        self.assertEqual(133, len({name for name, en, zh in MAPPED}))
        original = {(s["name"], s["english"]) for s in SPELLS}
        mapped = {(name, en) for name, en, zh in MAPPED}
        self.assertEqual(original, mapped)
        for name, en, zh in MAPPED:
            with self.subTest(spell=name):
                self.assertTrue(en.strip() and zh.strip())
                self.assertNotEqual(en, zh)
                self.assertNotIn("\n", zh)
                self.assertNotIn("\r", zh)

    def test_blackguard_levels_and_costs_are_visible(self):
        translations = {name: zh for name, en, zh in MAPPED
                        if (name, en) in BLACKGUARD}
        for name, token in (
            ("Seek Battle", "一回合"),
            ("Berserk Strength", "75"),
            ("Berserk Strength", "10"),
            ("Berserk Strength", "無法隨時間"),
            ("Whirlwind Attack", "15"),
            ("Whirlwind Attack", "20"),
            ("Whirlwind Attack", "35"),
            ("Whirlwind Attack", "50"),
            ("Shatter Stone", "20+1d30"),
            ("Leap into Battle", "最多 4"),
            ("Leap into Battle", "25"),
            ("Leap into Battle", "40"),
            ("Leap into Battle", "25%"),
            ("Grim Purpose", "12+d12"),
            ("Maim Foe", "15"),
            ("Maim Foe", "6 回合"),
            ("Venom", "18+d18"),
            ("Bloodlust", "副作用"),
            ("Unholy Reprieve", "66"),
            ("Unholy Reprieve", "STR"),
            ("Unholy Reprieve", "INT"),
            ("Unholy Reprieve", "CON"),
            ("Quake", "13"),
            ("Relentless Taunting", "法術"),
            ("Howl of the Damned", "抵抗恐懼"),
        ):
            with self.subTest(name=name, token=token):
                self.assertIn(token, translations[name])

    def test_two_known_cross_class_differences_still_distinct(self):
        by_pair = {(name, en): zh for name, en, zh in MAPPED}
        heroism = [(en, zh) for (name, en), zh in by_pair.items()
                   if name == "Heroism"]
        herbal = [(en, zh) for (name, en), zh in by_pair.items()
                  if name == "Herbal Curing"]
        self.assertEqual(2, len(heroism))
        self.assertEqual(2, len(herbal))
        self.assertEqual(2, len({zh for en, zh in heroism}))
        self.assertEqual(2, len({zh for en, zh in herbal}))
        priest = next(zh for en, zh in heroism if "character level 20" in en)
        paladin = next(zh for en, zh in heroism if "character level 15" in en)
        self.assertIn("20", priest)
        self.assertIn("15", paladin)
        ranger = next(zh for en, zh in herbal if "well-nourished" not in en)
        druid = next(zh for en, zh in herbal if "well-nourished" in en)
        self.assertNotIn("營養", ranger)
        self.assertIn("營養", druid)

    def test_native_rows_and_runner(self):
        pattern = re.compile(
            r'^\s*check\(("(?:[^"\\]|\\.)*"),\s*'
            r'("(?:[^"\\]|\\.)*"),\s*'
            r'("(?:[^"\\]|\\.)*")\);$', re.M)
        rows = {tuple(json.loads(value) for value in m.groups())
                for m in pattern.finditer(NATIVE)}
        expected = {(name, en, zh) for name, en, zh in MAPPED
                    if (name, en) in BLACKGUARD}
        self.assertEqual(15, len(rows))
        self.assertEqual(expected, rows)
        self.assertIn("tests/native/test_blackguard_spell_descriptions_locale.c",
                      RUNNER)
        self.assertIn("tests/test_blackguard_spell_descriptions_locale_source.py",
                      VERIFY)
        self.assertIn("unknown, 1) == unknown", NATIVE)


if __name__ == "__main__":
    unittest.main()
