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
        self.assertEqual(block.count("angdroid_symbol_query_format("), 2)
        self.assertIn('strnfmt(buf, max, "%c - %s.", sym, race->text);', block)
        self.assertIn("Unknown Symbol.", block)

    def test_english_and_unknown_fallback_remain(self):
        self.assertIn("if (!canonical || !zh_tw) return canonical;", HEADER)
        self.assertIn("return canonical;", HEADER)
        self.assertIn('period = (zh_tw && strcmp(label, canonical) != 0) ? "。" : ".";', HEADER)
        self.assertIn('snprintf(out, capacity, "%c - %s%s", sym, label, period)', HEADER)


if __name__ == "__main__":
    unittest.main()
