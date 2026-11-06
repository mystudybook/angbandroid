#!/usr/bin/env python3
"""Paladin and Rogue spell details, canonical source and cross-class matching."""
import importlib.util
import json
import re
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SPEC = importlib.util.spec_from_file_location(
    "angband_original_spell_description_source",
    ROOT / "tests/test_spell_descriptions_locale_source.py")
assert SPEC is not None and SPEC.loader is not None
SOURCE = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(SOURCE)
SPELLS = SOURCE.parsed_spells()
MAPPED = SOURCE.translated_entries(SOURCE.HELPER)
PALADIN = {(s["name"], s["english"]) for s in SPELLS if s["class"] == "Paladin"}
ROGUE = {(s["name"], s["english"]) for s in SPELLS if s["class"] == "Rogue"}
PRIEST = {(s["name"], s["english"]) for s in SPELLS if s["class"] == "Priest"}
MAGE = {(s["name"], s["english"]) for s in SPELLS if s["class"] == "Mage"}
DRUID = {(s["name"], s["english"]) for s in SPELLS if s["class"] == "Druid"}
NECROMANCER = {(s["name"], s["english"]) for s in SPELLS
               if s["class"] == "Necromancer"}
TEST = (ROOT / "tests/native/test_paladin_rogue_spell_descriptions_locale.c").read_text(
    encoding="utf-8")
RUNNER = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")


class PaladinRogueSpellDescriptionsSourceTest(unittest.TestCase):
    def test_complete_class_records_canonical_source(self):
        translated_pairs = {(n, en) for n, en, zh in MAPPED}
        ranger = {(s["name"], s["english"]) for s in SPELLS
                  if s["class"] == "Ranger"}
        blackguard = {(s["name"], s["english"]) for s in SPELLS
                      if s["class"] == "Blackguard"}
        self.assertEqual(15, len(blackguard))
        classes = (MAGE, DRUID, PRIEST, NECROMANCER, PALADIN, ROGUE,
                   ranger, blackguard)
        self.assertEqual(163, len(SPELLS))
        self.assertEqual(16, len(PALADIN))
        self.assertEqual(10, len(ROGUE))
        self.assertEqual(135, len(MAPPED))
        self.assertEqual(135, len(translated_pairs))
        self.assertEqual(133, len({n for n, en, zh in MAPPED}))
        self.assertEqual(set.union(*classes), translated_pairs)
        for name, english, chinese in MAPPED:
            with self.subTest(spell=name):
                self.assertTrue(english.strip() and chinese.strip())
                self.assertNotEqual(english, chinese)
                self.assertNotIn("\n", chinese)
                self.assertNotIn("\r", chinese)

    def test_paladin_heroism_special_15_vs_priest_20(self):
        priest = dict(PRIEST)
        paladin = dict(PALADIN)
        shared = set(priest) & set(paladin)
        same = {n for n in shared if priest[n] == paladin[n]}
        self.assertEqual(15, len(shared))
        self.assertEqual(14, len(same))
        self.assertEqual({"Heroism"}, shared - same)
        self.assertIn("character level 15", paladin["Heroism"])
        self.assertIn("character level 20", priest["Heroism"])
        translated = {(n, en): zh for n, en, zh in MAPPED}
        self.assertIn("等級達到 15", translated["Heroism", paladin["Heroism"]])
        self.assertIn("等級 20", translated["Heroism", priest["Heroism"]])
        self.assertNotEqual(translated["Heroism", paladin["Heroism"]],
                            translated["Heroism", priest["Heroism"]])

    def test_rogue_seven_reused_and_three_new(self):
        rogue = dict(ROGUE)
        mage = dict(MAGE)
        shared = set(rogue) & set(mage)
        self.assertEqual(7, len(shared))
        self.assertEqual({n for n in shared if rogue[n] == mage[n]}, shared)
        self.assertEqual({"Object Detection", "Detect Stairs", "Hit and Run"},
                         set(rogue) - shared)
        mapped = {(n, en): zh for n, en, zh in MAPPED}
        self.assertIn("偷竊", mapped["Hit and Run", rogue["Hit and Run"]])
        self.assertIn("傳送", mapped["Hit and Run", rogue["Hit and Run"]])
        self.assertIn("石室", mapped["Single Combat", dict(PALADIN)["Single Combat"]])

    def test_native_26_source_rows_and_runner_registration(self):
        pattern = re.compile(
            r'^\s*check\(("(?:[^"\\]|\\.)*"),\s*'
            r'("(?:[^"\\]|\\.)*"),\s*'
            r'("(?:[^"\\]|\\.)*")\);$', re.M)
        rows = {tuple(json.loads(part) for part in m.groups())
                for m in pattern.finditer(TEST)}
        expected = {(n, en, zh) for n, en, zh in MAPPED
                    if (n, en) in PALADIN | ROGUE}
        self.assertEqual(26, len(rows))
        self.assertEqual(expected, rows)
        self.assertIn('priest_heroism, 1),', TEST)
        self.assertIn('tests/native/test_paladin_rogue_spell_descriptions_locale.c',
                      RUNNER)
        self.assertIn('tests/test_paladin_rogue_spell_descriptions_locale_source.py',
                      VERIFY)


if __name__ == "__main__":
    unittest.main()
