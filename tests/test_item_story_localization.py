#!/usr/bin/env python3
"""Protect on-screen artifact/ego narrative localization and data isolation."""
from pathlib import Path
import re
import unittest

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp/angband"
SOURCE = (CPP / "obj-info.c").read_text(encoding="utf-8")
ARTIFACT = (CPP / "zh-tw-artifact-lore.h").read_text(encoding="utf-8")
EGO = (CPP / "zh-tw-ego-lore.h").read_text(encoding="utf-8")
KIND = (CPP / "zh-tw-kind-lore.h").read_text(encoding="utf-8")


class ItemStoryLocalizationTest(unittest.TestCase):
    def test_canonical_translations_are_exact_and_bounded(self):
        pattern = re.compile(r'\{\s*"((?:\\.|[^"\\])*)",\s*"((?:\\.|[^"\\])*)"\s*\}')
        art = pattern.findall(ARTIFACT)
        ego = pattern.findall(EGO)
        kind = pattern.findall(KIND)
        self.assertEqual(len(kind), 141)
        self.assertEqual(len({name for name, _ in kind}), 141)
        self.assertEqual(len(art), 138)
        self.assertEqual(len({name for name, _ in art}), 138)
        self.assertEqual(len(ego), 5)
        self.assertEqual(len({name for name, _ in ego}), 5)

    def test_artifact_lore_only_when_revealed_and_not_randart(self):
        self.assertIn("!OPT(player, birth_randarts) && obj->artifact &&", SOURCE)
        self.assertIn("obj->known->artifact && obj->artifact->text", SOURCE)
        self.assertIn("angdroid_zh_tw_artifact_flavor(obj->artifact->name)", SOURCE)
        self.assertIn("const char *flavor = obj->artifact->text;", SOURCE)
        self.assertIn("if (translated) flavor = translated;", SOURCE)
        self.assertIn('textblock_append(tb, "%s\\n\\n", flavor);', SOURCE)

    def test_item_kind_lore_only_after_original_awareness_guard(self):
        self.assertIn('if (obj->kind != obj->known->kind)', SOURCE)
        self.assertIn('object_flavor_is_aware(obj) || ego', SOURCE)
        self.assertIn('if (!ego && obj->kind->text)', SOURCE)
        self.assertIn('const char *flavor = obj->kind->text;', SOURCE)
        self.assertIn('angdroid_zh_tw_kind_flavor(obj->kind->name)', SOURCE)
        self.assertIn('if (translated) flavor = translated;', SOURCE)
        self.assertIn('textblock_append(tb, "%s", flavor);', SOURCE)
        self.assertIn('#include "zh-tw-kind-lore.h"', SOURCE)

    def test_ego_lore_does_not_disclose_unidentified_ego(self):
        self.assertIn("obj->known->ego != NULL", SOURCE)
        self.assertIn("obj->ego &&", SOURCE)
        self.assertIn("angdroid_zh_tw_ego_flavor(obj->ego->name)", SOURCE)
        self.assertIn("const char *flavor = obj->ego->text;", SOURCE)
        self.assertIn("if (did_desc) textblock_append(tb, \"  \");", SOURCE)
        self.assertIn("if (translated) flavor = translated;", SOURCE)

    def test_dump_and_spoilers_keep_original_english(self):
        self.assertIn("angdroid_zh_tw_enabled() && !(mode & OINFO_SPOIL)", SOURCE)
        self.assertIn("if (!terse) describe_flavor_text(tb, obj, ego,", SOURCE)
        self.assertIn("object_info_out(obj, OINFO_TERSE | OINFO_SUBJ)", SOURCE)
        self.assertIn("object_info_out(obj, OINFO_SPOIL)", SOURCE)
        self.assertIn('return NULL;', ARTIFACT)
        self.assertIn('return NULL;', EGO)


if __name__ == "__main__":
    unittest.main()
