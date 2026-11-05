#!/usr/bin/env python3
"""Known item classifiers, Chinese suffixes and UTF-8 append are display-only."""
import re
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp/angband"
CANONICAL = (CPP / "list-tvals.h").read_text(encoding="utf-8")
HELPERS = (CPP / "zh-tw-item-grammar.h").read_text(encoding="utf-8")
SOURCE = (CPP / "obj-desc.c").read_text(encoding="utf-8")
EXPORT = (CPP / "save-charoutput.c").read_text(encoding="utf-8")
RAND = (CPP / "obj-util.c").read_text(encoding="utf-8")


class KnownItemGrammarTest(unittest.TestCase):
    def test_all_canonical_tvals_are_classified_in_source_order(self):
        source_names = re.findall(r'^TV\(\w+,\s*"([^"]+)"\)', CANONICAL, re.M)
        classifications = re.findall(r'^\s*\{ "([^"]+)", "([^"]+)" \},?$', HELPERS, re.M)
        self.assertEqual(len(source_names), 36)
        self.assertEqual([name for name, _ in classifications], source_names)
        self.assertEqual(len(set(source_names)), 36)
        self.assertTrue(all(unit for _, unit in classifications))

    def test_only_opted_in_translated_names_use_the_helpers(self):
        self.assertIn('#include "zh-tw-item-grammar.h"', SOURCE)
        self.assertIn('if (localized_kind)', SOURCE)
        self.assertIn("angdroid_zh_tw_known_item_name(", SOURCE)
        self.assertIn("angdroid_zh_tw_item_append_utf8(buf, max, &end,", SOURCE)
        self.assertIn("obj->kind->flavor &&", SOURCE)
        self.assertIn("angdroid_zh_tw_identified_flavor_name(named_effect,", SOURCE)
        self.assertIn("tval_find_name(obj->tval),", SOURCE)
        self.assertIn("display_kind = named_effect;", SOURCE)
        self.assertIn("tval_find_name(obj->tval),", SOURCE)
        self.assertIn("display_kind, prefix, number))", SOURCE)
        self.assertIn("obj_desc_name_format(buf, max, end, basename, modstr, plural)", SOURCE)
        self.assertIn("(mode & ODESC_ZH_TW) && angdroid_zh_tw_enabled()", SOURCE)
        self.assertNotIn("zh-tw-item-grammar.h", EXPORT)
        self.assertNotIn("zh-tw-item-grammar.h", RAND)

    def test_ego_and_artifact_require_identity_and_translated_base(self):
        self.assertIn("if (object_is_known_artifact(obj))", SOURCE)
        self.assertIn("obj->known->ego && !(mode & ODESC_NOEGO)", SOURCE)
        self.assertIn("obj->ego && store", SOURCE)
        self.assertIn("if (localized) artifact_name = localized;", SOURCE)
        self.assertIn("if (localized) ego_name = localized;", SOURCE)
        self.assertEqual(SOURCE.count("if (localized_kind && translated_suffix)"), 2)
        self.assertEqual(SOURCE.count("angdroid_zh_tw_item_suffix("), 2)
        self.assertIn('strnfcat(buf, max, &end, " %s", artifact_name);', SOURCE)
        self.assertIn('strnfcat(buf, max, &end, " %s", ego_name);', SOURCE)
        self.assertIn("angdroid_zh_tw_artifact_name(artifact_name)", SOURCE)
        self.assertIn("angdroid_zh_tw_ego_name(ego_name)", SOURCE)

    def test_all_or_nothing_text_before_clipping(self):
        self.assertIn("if (a >= cap || b >= cap - a) return false;", HELPERS)
        self.assertIn("if (width > length - pos || width > limit - pos) break;", HELPERS)
        self.assertIn("out[*end + pos] = '\\0';", HELPERS)
        self.assertIn("memcpy(out + *end + pos, text + pos, width);", HELPERS)


if __name__ == "__main__":
    unittest.main()
