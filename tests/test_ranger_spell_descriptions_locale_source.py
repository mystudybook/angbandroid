#!/usr/bin/env python3
"""Canonical Ranger spell descriptions and dual Druid Herbal Curing."""
from pathlib import Path
import importlib.util
import json
import re
import unittest

ROOT = Path(__file__).resolve().parents[1]
SPEC = importlib.util.spec_from_file_location(
    "angband_spell_description_source",
    ROOT / "tests/test_spell_descriptions_locale_source.py")
assert SPEC is not None and SPEC.loader is not None
SOURCE = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(SOURCE)
SPELLS = SOURCE.parsed_spells()
MAPPED = SOURCE.translated_entries(SOURCE.HELPER)
RANGER = {(x["name"], x["english"]) for x in SPELLS if x["class"] == "Ranger"}
DRUID = {(x["name"], x["english"]) for x in SPELLS if x["class"] == "Druid"}
NATIVE = (ROOT / "tests/native/test_ranger_spell_descriptions_locale.c").read_text(
    encoding="utf-8")
RUNNER = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")


class RangerDescriptionsLocaleSourceTest(unittest.TestCase):
    def test_canonical_ranger_all_eleven_are_covered(self):
        ranger = dict(RANGER)
        druid = dict(DRUID)
        mapped = {(n, en): zh for n, en, zh in MAPPED}
        self.assertEqual(163, len(SPELLS))
        self.assertEqual(11, len(RANGER))
        self.assertEqual(120, len(MAPPED))
        self.assertEqual(120, len(mapped))
        self.assertEqual(118, len({name for name, en, zh in MAPPED}))
        self.assertTrue(RANGER.issubset(set(mapped)))
        shared = set(ranger) & set(druid)
        self.assertEqual(7, len(shared))
        self.assertEqual(6, sum(ranger[n] == druid[n] for n in shared))
        self.assertEqual({"Herbal Curing"},
                         {n for n in shared if ranger[n] != druid[n]})
        self.assertNotEqual(mapped["Herbal Curing", ranger["Herbal Curing"]],
                            mapped["Herbal Curing", druid["Herbal Curing"]])

    def test_ranger_effects_and_restrictions(self):
        mapped = {(n, en): zh for n, en, zh in MAPPED}
        zh = {n: mapped[n, en] for n, en in RANGER}
        self.assertIn("中毒", zh["Herbal Curing"])
        self.assertIn("黑色吐息", zh["Herbal Curing"])
        self.assertNotIn("營養", zh["Herbal Curing"])
        for name, token in (
            ("Cover Tracks", "氣味"),
            ("Cover Tracks", "等級"),
            ("Create Arrows", "法杖"),
            ("Create Arrows", "箭矢"),
            ("Decoy", "聲音"),
            ("Brand Ammunition", "火焰"),
            ("Brand Ammunition", "寒冷"),
            ("Brand Ammunition", "毒素"),
            ("Brand Ammunition", "隨機"),
            ("Brand Ammunition", "命中"),
            ("Brand Ammunition", "傷害"),
            ("Brand Ammunition", "已損壞"),
            ("Brand Ammunition", "詛咒"),
            ("Brand Ammunition", "不會生效"),
        ):
            with self.subTest(name=name, token=token):
                self.assertIn(token, zh[name])

    def test_native_and_source_coverage_registered(self):
        regex = re.compile(
            r'^\s*check\(("(?:[^"\\]|\\.)*"),\s*'
            r'("(?:[^"\\]|\\.)*"),\s*'
            r'("(?:[^"\\]|\\.)*")\);$', re.M)
        rows = {tuple(json.loads(part) for part in m.groups())
                for m in regex.finditer(NATIVE)}
        mapped = {(n, en): zh for n, en, zh in MAPPED}
        expected = {(n, en, mapped[n, en]) for n, en in RANGER}
        self.assertEqual(11, len(rows))
        self.assertEqual(expected, rows)
        self.assertIn('ranger_herbal, 1),', NATIVE)
        self.assertIn('druid_herbal, 1)) != 0', NATIVE)
        self.assertIn('test_ranger_spell_descriptions_locale.c', RUNNER)
        self.assertIn('tests/test_ranger_spell_descriptions_locale_source.py',
                      VERIFY)


if __name__ == "__main__":
    unittest.main()
