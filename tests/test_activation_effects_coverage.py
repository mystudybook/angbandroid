#!/usr/bin/env python3
"""Audit canonical activation/property description localization source coverage.

The localized text is display-only: activation IDs, effect data, and gameplay
remain English. This audit intentionally does not claim 100% game translation.
"""
from __future__ import annotations

import re
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp/angband"
DATA = ROOT / "variant_lib/angband/gamedata"


def source_entries(path: Path, prefix: str) -> dict[str, list[str]]:
    records: dict[str, list[str]] = {}
    current: str | None = None
    for line in path.read_text(encoding="utf-8").splitlines():
        if line.startswith("name:"):
            current = line.partition(":")[2]
            records[current] = []
        elif line.startswith(prefix + ":") and current is not None:
            records[current].append(line.partition(":")[2])
    return records


def translated_ids(path: Path) -> list[str]:
    source = path.read_text(encoding="utf-8")
    return re.findall(r'\{\s*"([A-Z][A-Z0-9_]*)",\s*"(.*?)"\s*\}', source)


class ActivationEffectsCoverageTest(unittest.TestCase):
    def test_activation_descriptions_exact_source_coverage(self):
        records = source_entries(DATA / "activation.txt", "desc")
        canonical = {name for name, desc in records.items() if desc}
        translated = dict(translated_ids(CPP / "zh-tw-activations.h"))
        self.assertEqual(len(canonical), 116)
        self.assertEqual(len(translated), 116)
        self.assertEqual(set(translated), canonical)
        self.assertTrue(all(any(ord(ch) > 127 for ch in text)
                            for text in translated.values()))
        header = (CPP / "zh-tw-activations.h").read_text(encoding="utf-8")
        self.assertIn("if (!id || !zh_tw) return NULL;", header)
        self.assertIn("return NULL;", header)

    def test_object_property_descriptions_exact_source_coverage(self):
        descriptions = {
            line[5:] for line in (DATA / "object_property.txt")
            .read_text(encoding="utf-8").splitlines() if line.startswith("desc:")
        }
        header = (CPP / "zh-tw-item-abilities.h").read_text(encoding="utf-8")
        translations = dict(re.findall(
            r'\{\s*"([^"\n]+)",\s*"([^"\n]+)"\s*\}', header
        ))
        self.assertEqual(len(descriptions), 22)
        self.assertEqual(descriptions, set(translations))

    def test_existing_curse_dictionary_covers_curse_data(self):
        descriptions = {
            line[5:] for line in (DATA / "curse.txt")
            .read_text(encoding="utf-8").splitlines() if line.startswith("desc:")
        }
        header = (CPP / "zh-tw-curse.h").read_text(encoding="utf-8")
        matches = re.search(
            r'angdroid_curse_description\(.*?\)\s*\{(.*?)\n\}',
            header, re.S
        )
        self.assertIsNotNone(matches)
        translated = {
            key for key in re.findall(r'\{\s*"([^"]+)"\s*,\s*"[^"]+"\s*\}',
                                      matches.group(1))
        }
        self.assertEqual(descriptions, translated)

    def test_obj_info_keeps_identification_and_english_fallback(self):
        src = (CPP / "obj-info.c").read_text(encoding="utf-8")
        self.assertIn("if (obj->activation && obj->activation->desc)", src)
        self.assertIn("angdroid_zh_tw_activation_description(", src)
        self.assertIn("obj->activation->name, angdroid_zh_tw_enabled()", src)
        self.assertIn('textblock_append(tb, "When activated, it ");', src)
        self.assertIn('localized_activation ? "。\\n" : ".\\n"', src)
        self.assertIn("angdroid_curse_description(curses[i].desc, true)", src)
        self.assertIn("angdroid_zh_tw_item_ability(prop->desc,", src)
        self.assertIn('angdroid_item_info_text("It can be eaten.', src)


    def test_all_visible_item_origins_have_chinese_display_paths(self):
        origin_source = (CPP / "list-origins.h").read_text(encoding="utf-8")
        entries = re.findall(
            r'ORIGIN\(\s*(\w+),\s*(-?\d+),', origin_source
        )
        visible = {name for name, count in entries if int(count) >= 0}
        self.assertGreaterEqual(len(visible), 15)
        src = (CPP / "obj-info.c").read_text(encoding="utf-8")
        missing = [name for name in visible if "case ORIGIN_" + name + ":" not in src]
        self.assertEqual(missing, [], "visible origin case missing")
        self.assertIn('"地城 %d 英呎（第 %d 層）"', src)
        self.assertIn("angdroid_zh_tw_monster_list_name(dropper)", src)
        self.assertIn('textblock_append(tb, "由%s在%s掉落"', src)
        self.assertIn('textblock_append(tb, "從商店購得");', src)
        self.assertIn("handled = false;", src)
        self.assertIn("textblock_append(tb, origins[origin].desc, name, loot_spot)", src)


if __name__ == "__main__":
    unittest.main()
