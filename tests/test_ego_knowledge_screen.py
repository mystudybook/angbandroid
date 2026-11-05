#!/usr/bin/env python3
"""Ego knowledge names are display-only, bounded and reversible."""
from pathlib import Path
import re
import unittest

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp/angband"
SOURCE = (CPP / "ui-knowledge.c").read_text(encoding="utf-8")
DICT = (CPP / "zh-tw-ego-names.h").read_text(encoding="utf-8")


class EgoKnowledgeScreenTest(unittest.TestCase):
    def test_79_canonical_ego_label_entries(self):
        entries = re.findall(r'\{\s*"([^"\n]+)",\s*"([^"\n]+)"\s*\}', DICT)
        self.assertEqual(len(entries), 79)
        self.assertEqual(len({name for name, _ in entries}), 79)

    def test_list_uses_display_only_translation_and_cell_clipping(self):
        start = SOURCE.index("static void display_ego_item(")
        end = SOURCE.index("static void desc_ego_fake(", start)
        block = SOURCE[start:end]
        self.assertIn('angdroid_zh_tw_ego_name(ego->name)', block)
        self.assertIn('const char *display_name = localized ? localized : ego->name;', block)
        self.assertIn('if (Term && col >= 0 && col < Term->wid)', block)
        self.assertIn('angdroid_knowledge_clip_label(clipped, sizeof(clipped),', block)
        self.assertIn('display_name, col, Term->wid, Term->wid', block)
        self.assertIn('c_prt(attr, clipped, row, col)', block)
        self.assertNotIn('c_prt(attr, ego->name, row, col)', block)

    def test_recall_title_uses_same_dictionary_and_group_fallback(self):
        start = SOURCE.index("static void desc_ego_fake(")
        end = SOURCE.index("static int e_cmp_tval(", start)
        block = SOURCE[start:end]
        self.assertIn('angdroid_zh_tw_ego_name(ego->name)', block)
        self.assertIn('angdroid_knowledge_group_text(', block)
        self.assertIn('textui_textblock_show(tb, area, format("%s %s", group, display_name));', block)
        self.assertIn('textblock_free(tb);', block)


if __name__ == "__main__":
    unittest.main()
