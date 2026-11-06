#!/usr/bin/env python3
"""Canonical Druid descriptions; cover same-name Ranger differences and warnings."""
import importlib.util
import json
import re
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SPEC = importlib.util.spec_from_file_location(
    "mage_spell_descriptions", ROOT / "tests/test_spell_descriptions_locale_source.py")
assert SPEC is not None and SPEC.loader is not None
MODULE = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(MODULE)
RECORDS = MODULE.parsed_spells()
MAPPED = MODULE.translated_entries(MODULE.HELPER)
DRUID = {(x["name"], x["english"]) for x in RECORDS if x["class"] == "Druid"}
MAGE = {(x["name"], x["english"]) for x in RECORDS if x["class"] == "Mage"}
RANGER = {x["name"]: x["english"] for x in RECORDS if x["class"] == "Ranger"}
NATIVE = (ROOT / "tests/native/test_druid_spell_descriptions_locale.c").read_text(
    encoding="utf-8")
RUNNER = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")


class DruidSpellDescriptionsLocaleSourceTest(unittest.TestCase):
    def test_complete_exact_druid_coverage_and_mage_unchanged(self):
        self.assertEqual(163, len(RECORDS))
        self.assertEqual(30, len(MAGE))
        self.assertEqual(27, len(DRUID))
        self.assertTrue(MAGE.isdisjoint(DRUID))
        # Dictionary now includes the original Priest spells as well.
        priest = {(x["name"], x["english"]) for x in RECORDS
                  if x["class"] == "Priest"}
        self.assertEqual(28, len(priest))
        self.assertEqual(120, len(MAPPED))
        self.assertEqual(120, len({(n, en) for n, en, zh in MAPPED}))
        necromancer = {(x["name"], x["english"]) for x in RECORDS
                       if x["class"] == "Necromancer"}
        self.assertEqual(26, len(necromancer))
        paladin = {(x["name"], x["english"]) for x in RECORDS
                   if x["class"] == "Paladin"}
        rogue = {(x["name"], x["english"]) for x in RECORDS
                 if x["class"] == "Rogue"}
        ranger = {(x["name"], x["english"]) for x in RECORDS
                  if x["class"] == "Ranger"}
        self.assertEqual(11, len(ranger))
        self.assertEqual(MAGE | DRUID | priest | necromancer
                         | paladin | rogue | ranger,
                         {(n, en) for n, en, zh in MAPPED})
        for name, english, chinese in MAPPED:
            with self.subTest(name=name):
                self.assertTrue(english.strip() and chinese.strip())
                self.assertNotEqual(english, chinese)
                self.assertNotIn("\n", chinese)
                self.assertNotIn("\r", chinese)

    def test_six_shared_ranger_spells_and_one_exception(self):
        druid = dict(DRUID)
        shared = set(druid) & set(RANGER)
        self.assertEqual(7, len(shared))
        exact = {n for n in shared if druid[n] == RANGER[n]}
        self.assertEqual(6, len(exact))
        self.assertEqual({"Herbal Curing"}, shared - exact)
        self.assertIn("well-nourished", druid["Herbal Curing"])
        self.assertNotIn("well-nourished", RANGER["Herbal Curing"])
        mapped = {(n, en): zh for n, en, zh in MAPPED}
        druid_herbs = mapped["Herbal Curing", druid["Herbal Curing"]]
        ranger_herbs = mapped["Herbal Curing", RANGER["Herbal Curing"]]
        self.assertNotEqual(druid_herbs, ranger_herbs)
        self.assertIn("營養", druid_herbs)
        self.assertNotIn("營養", ranger_herbs)

    def test_sensitive_warning_and_numeric_translation(self):
        zh = {n: text for n, en, text in MAPPED if (n, en) in DRUID}
        for name, token in (
            ("Fox Form", "無法使用物品"),
            ("Resist Poison", "20+1d20"),
            ("Turn Stone to Mud", "20+1d30"),
            ("Lightning Strike", "玩家所在位置"),
            ("Tremor", "4"),
            ("Tremor", "城鎮"),
            ("Tremor", "神器"),
            ("Haste Self", "10"),
            ("Rapid Regeneration", "5+1d3"),
            ("Herbal Curing", "營養"),
            ("Meteor Swarm", "3"),
            ("Meteor Swarm", "4"),
            ("Meteor Swarm", "半徑 1"),
            ("Volcanic Eruption", "半徑 5"),
        ):
            with self.subTest(name=name, token=token):
                self.assertIn(token, zh[name])

    def test_exact_native_coverage_and_registration(self):
        matcher = re.compile(
            r'^\s*check\(("(?:[^"\\]|\\.)*"),\s*'
            r'("(?:[^"\\]|\\.)*"),\s*'
            r'("(?:[^"\\]|\\.)*")\);$', re.M)
        rows = [tuple(json.loads(p) for p in m.groups())
                for m in matcher.finditer(NATIVE)]
        self.assertEqual(27, len(rows))
        self.assertEqual({(n, en, zh) for n, en, zh in MAPPED
                          if (n, en) in DRUID}, set(rows))
        self.assertIn("ranger_herbs, 1),", NATIVE)
        self.assertIn("druid_herbs, 1)) != 0", NATIVE)
        self.assertIn("tests/native/test_druid_spell_descriptions_locale.c", RUNNER)
        self.assertIn("tests/test_druid_spell_descriptions_locale_source.py", VERIFY)


if __name__ == "__main__":
    unittest.main()
