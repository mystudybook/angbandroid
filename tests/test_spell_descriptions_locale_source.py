#!/usr/bin/env python3
"""Exact display-only localization of original Angband Mage spell descriptions."""
from pathlib import Path
import json
import re
import unittest

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp/angband"
GAME = (ROOT / "variant_lib/angband/gamedata/class.txt").read_text(encoding="utf-8")
HELPER = (CPP / "zh-tw-spell-descriptions.h").read_text(encoding="utf-8")
UI = (CPP / "ui-spell.c").read_text(encoding="utf-8")
PARSER = (CPP / "init.c").read_text(encoding="utf-8")
NATIVE = (ROOT / "tests/native/test_spell_descriptions_locale.c").read_text(encoding="utf-8")
RUNNER = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")


def parsed_spells():
    entries = []
    cls = None
    spell = None
    for line in GAME.splitlines():
        if line.startswith("name:"):
            cls, spell = line[5:], None
        elif line.startswith("spell:"):
            spell = {"class": cls, "name": line[6:].split(":", 1)[0],
                     "english": ""}
            entries.append(spell)
        elif line.startswith("desc:") and spell:
            spell["english"] += line[5:]
    return entries


def translated_entries(src):
    pattern = re.compile(
        r'^\s*\{\s*("(?:[^"\\]|\\.)*"),\s*("(?:[^"\\]|\\.)*"),'
        r'\s*("(?:[^"\\]|\\.)*")\s*\},\s*$', re.M)
    return [tuple(json.loads(part) for part in match.groups())
            for match in pattern.finditer(src)]


class SpellDescriptionsLocaleSourceTest(unittest.TestCase):
    def test_exact_original_mage_description_mapping(self):
        mage = [(entry["name"], entry["english"]) for entry in parsed_spells()
                if entry["class"] == "Mage"]
        mapped = translated_entries(HELPER)
        self.assertEqual(30, len(mage))
        self.assertEqual(30, len(mapped))
        self.assertEqual(len(set(mage)), 30)
        self.assertEqual(set(mage), {(n, english) for n, english, _ in mapped})
        self.assertEqual(len({n for n, _, _ in mapped}), 30)
        self.assertTrue(all(chinese.strip() for _, _, chinese in mapped))
        self.assertTrue(all(chinese != english for _, english, chinese in mapped))
        self.assertTrue(all(not re.search(r"[\\r\\n]", chinese)
                            for _, _, chinese in mapped))

    def test_shared_rogue_spells_have_identical_canonical_descriptions(self):
        records = parsed_spells()
        mage = {e["name"]: e["english"] for e in records if e["class"] == "Mage"}
        rogue = {e["name"]: e["english"] for e in records if e["class"] == "Rogue"}
        shared = set(mage) & set(rogue)
        self.assertEqual(7, len(shared))
        for name in shared:
            with self.subTest(name=name):
                self.assertEqual(mage[name], rogue[name])

    def test_exact_matching_guards_future_edits_and_mode_switch(self):
        self.assertIn("if (!english || !zh_tw || !name) return english;", HELPER)
        self.assertIn("strcmp(name, map[i].name) == 0", HELPER)
        self.assertIn("strcmp(english, map[i].en) == 0", HELPER)
        self.assertIn("return english;", HELPER)
        self.assertIn("string_append(spell->text, parser_getstr(p, \"desc\"))", PARSER)
        self.assertIn('#include "zh-tw-spell-descriptions.h"', UI)
        self.assertIn('text_out("\\n%s", angdroid_zh_tw_spell_description(', UI)
        self.assertIn("spell->name, spell->text, angdroid_zh_tw_enabled()));", UI)
        self.assertIn("text_out_hook = text_out_to_screen;", UI)
        self.assertIn("const char *projection = angdroid_projection_description(", UI)
        self.assertIn("effect_avg_damage(e, shared_dice)", UI)
        self.assertNotIn("spell->text =", UI)

    def test_native_tests_match_source_and_are_registered(self):
        mage = [(e["name"], e["english"]) for e in parsed_spells()
                if e["class"] == "Mage"]
        checks = re.findall(r'^\s*check\((.+)\);$', NATIVE, re.M)
        self.assertEqual(30, len(checks))
        parsed = []
        pattern = re.compile(
            r'^\s*check\(("(?:[^"\\]|\\.)*"),\s*'
            r'("(?:[^"\\]|\\.)*"),\s*'
            r'("(?:[^"\\]|\\.)*")\);$', re.M)
        for match in pattern.finditer(NATIVE):
            parsed.append(tuple(json.loads(part) for part in match.groups()))
        self.assertEqual(30, len(parsed))
        self.assertEqual(set(translated_entries(HELPER)), set(parsed))
        self.assertEqual(set(mage), {(n, en) for n, en, _ in parsed})
        self.assertIn("tests/native/test_spell_descriptions_locale.c", RUNNER)
        self.assertIn("tests/test_spell_descriptions_locale_source.py", VERIFY)


if __name__ == "__main__":
    unittest.main()
