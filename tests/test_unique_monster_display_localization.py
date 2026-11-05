#!/usr/bin/env python3
"""Unique monster zh-TW names: source completeness and display-only isolation."""
from __future__ import annotations

import re
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp/angband"


def canonical_uniques():
    source = (ROOT / "variant_lib/angband/gamedata/monster.txt").read_text(
        encoding="utf-8")
    names = set()
    current = None
    unique = False
    for line in source.splitlines():
        if line.startswith("name:"):
            if current and unique:
                names.add(current)
            current = line[5:]
            unique = False
        elif current and line.startswith("flags:") and re.search(
                r"\bUNIQUE\b", line):
            unique = True
    if current and unique:
        names.add(current)
    return names


def translations():
    header = (CPP / "zh-tw-unique-monsters.h").read_text(encoding="utf-8")
    pairs = re.findall(r'^\s*\{ "([^"]+)", "([^"]+)" \},$',
                       header, re.MULTILINE)
    return header, pairs


class UniqueMonsterDisplayTest(unittest.TestCase):
    def test_canonical_one_to_one_table(self):
        names = canonical_uniques()
        header, pairs = translations()
        self.assertEqual(len(names), 101)
        self.assertIn("#define ANGDROID_ZH_TW_UNIQUE_MONSTER_NAMES 101",
                      header)
        self.assertEqual(len(pairs), len(names))
        self.assertEqual({en for en, _ in pairs}, names)
        self.assertEqual(len({en for en, _ in pairs}), len(pairs))
        self.assertTrue(all(zh.strip() and re.search(r"[\u3400-\u9fff]", zh)
                            for _, zh in pairs))

    def test_famous_uniques_and_shapeshifts(self):
        _, pairs = translations()
        table = dict(pairs)
        self.assertEqual(table["Morgoth, Lord of Darkness"], "黑暗魔君魔苟斯")
        self.assertEqual(table["Sauron, the Sorcerer"], "巫師索倫")
        self.assertEqual(table["Wolf-Sauron"], "狼形索倫")
        self.assertEqual(table["Serpent-Sauron"], "巨蛇形索倫")
        self.assertEqual(table["Vampire-Sauron"], "吸血鬼形索倫")
        self.assertEqual(table["The Witch-King of Angmar"], "安格瑪巫王")

    def test_visibility_paths_and_english_fallback(self):
        mon = (CPP / "mon-desc.c").read_text(encoding="utf-8")
        knowledge = (CPP / "knowledge-monster-label.h").read_text(
            encoding="utf-8")
        recall = (CPP / "ui-mon-lore.c").read_text(encoding="utf-8")
        messages = (CPP / "mon-msg.c").read_text(encoding="utf-8")
        self.assertIn('strnfmt(buf, buflen, "[U] %s", localized ? localized : race->name)', mon)
        self.assertIn("is_unique ? angdroid_zh_tw_unique_monster_name(english)",
                      knowledge)
        self.assertIn("if (!english || !zh_tw) return english;", knowledge)
        self.assertIn("angdroid_zh_tw_unique_monster_name(race->name)", recall)
        self.assertIn("angdroid_zh_tw_unique_monster_name(race->name)", messages)
        self.assertIn("if (invisible)", messages)
        self.assertIn("if (count == 1) my_strcpy(buf, \"它\", buflen);", messages)
        self.assertIn("if (monster_is_shape_unique(mon))", mon)
        self.assertIn("my_strcpy(desc, mon->race->name, max);", mon)
        self.assertNotIn("mon->race->name =", mon)

    def test_all_known_monsters_have_separate_display_names(self):
        standard = (CPP / "zh-tw-monsters.h").read_text(encoding="utf-8")
        normal = {en for en, _ in re.findall(
            r'^\s*\{ "([^"]+)", "([^"]+)" \},$',
            standard, re.MULTILINE)}
        _, pairs = translations()
        unique = {en for en, _ in pairs}
        self.assertFalse(normal & unique)
        self.assertEqual(len(normal), 522)
        self.assertEqual(len(unique), 101)


if __name__ == "__main__":
    unittest.main()
