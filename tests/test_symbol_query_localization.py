#!/usr/bin/env python3
"""Symbol-query localization only changes screen text, not match priority."""
from pathlib import Path
import re
import unittest

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp/angband"
SOURCE = (CPP / "ui-knowledge.c").read_text(encoding="utf-8")
HEADER = (CPP / "zh-tw-symbol-query.h").read_text(encoding="utf-8")
TVALS = (CPP / "list-tvals.h").read_text(encoding="utf-8")
MONSTER_BASE = (ROOT / "variant_lib/angband/gamedata/monster_base.txt").read_text(encoding="utf-8")


class SymbolQueryLocalizationTest(unittest.TestCase):
    def test_all_original_tval_names_and_no_duplicates(self):
        canonical = re.findall(r'^TV\(\w+,\s*"([^"]+)"\)', TVALS, re.M)
        localized = re.findall(r'\{ "([^"]+)", "([^"]+)" \}', HEADER)
        self.assertEqual(len(canonical), 36)
        self.assertEqual([name for name, _ in localized], canonical)
        self.assertEqual(len({name for name, _ in localized}), len(canonical))
        self.assertTrue(all(text for _, text in localized))

    def test_lookup_preserves_match_order_and_canonical_labels(self):
        start = SOURCE.index("static void lookup_symbol(")
        end = SOURCE.index("void do_cmd_query_symbol(", start)
        block = SOURCE[start:end]
        self.assertLess(block.index("/* Look through items */"),
                        block.index("/* Look through features */"))
        self.assertLess(block.index("/* Look through features */"),
                        block.index("/* Look through monster templates */"))
        self.assertIn("char_matches_key(k_info[i].d_char, sym)", block)
        self.assertIn("char_matches_key(f_info[i].d_char, sym)", block)
        self.assertIn("angdroid_symbol_tval_name(", block)
        self.assertIn("angdroid_terrain_name(", block)
        self.assertEqual(block.count("angdroid_symbol_query_format("), 3)
        self.assertIn("angdroid_symbol_monster_base_text(", block)
        self.assertIn("race->name, race->text, angdroid_zh_tw_enabled()", block)
        self.assertIn('if (streq(race->name, "Morgoth")) continue;', block)
        self.assertNotIn('strnfmt(buf, max, "%c - %s.", sym, race->text);', block)
        self.assertIn("Unknown Symbol.", block)

    def test_english_and_unknown_fallback_remain(self):
        self.assertIn("if (!canonical || !zh_tw) return canonical;", HEADER)
        self.assertIn("return canonical;", HEADER)
        self.assertIn('period = (zh_tw && strcmp(label, canonical) != 0) ? "。" : ".";', HEADER)
        self.assertIn('snprintf(out, capacity, "%c - %s%s", sym, label, period)', HEADER)

    def test_all_canonical_monster_base_descriptions(self):
        records, name, description = [], None, ""
        for line in MONSTER_BASE.splitlines():
            if line.startswith("name:"):
                if name is not None:
                    records.append((name, description))
                name, description = line[5:], ""
            elif name is not None and line.startswith("desc:"):
                description += line[5:]
        if name is not None:
            records.append((name, description))
        translations = re.findall(
            r'^\s*\{ "([^"]+)", "([^"]+)", "([^"]+)" \},?$',
            HEADER, re.M
        )
        self.assertEqual(len(records), 56)
        self.assertEqual([(name, desc) for name, desc, _ in translations], records)
        self.assertEqual(len(set(name for name, _ in records)), 56)
        self.assertTrue(all(desc and zh for _, desc, zh in translations))
        self.assertIn(("Morgoth", "Morgoth", "魔苟斯"), translations)

    def test_monster_base_requires_source_identity_and_exact_text(self):
        self.assertIn("if (!base_name || !canonical_text || !zh_tw) return canonical_text;", HEADER)
        self.assertIn("strcmp(base_name, descriptions[i].base_name) == 0", HEADER)
        self.assertIn("strcmp(canonical_text, descriptions[i].canonical_text) == 0", HEADER)
        self.assertIn("return descriptions[i].traditional;", HEADER)
        self.assertIn("return canonical_text;", HEADER)


if __name__ == "__main__":
    unittest.main()
