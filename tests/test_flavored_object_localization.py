#!/usr/bin/env python3
"""Chinese noun phrases remain screen-only and never reveal unknown kind data."""
from pathlib import Path
import re
import unittest

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp/angband"
DESC = (CPP / "obj-desc.c").read_text(encoding="utf-8")
HEADER = (CPP / "zh-tw-flavored-object.h").read_text(encoding="utf-8")
EXPORT = (CPP / "save-charoutput.c").read_text(encoding="utf-8")
RNG = (CPP / "obj-util.c").read_text(encoding="utf-8")


class FlavoredObjectTest(unittest.TestCase):
    def test_full_seven_type_catalog(self):
        types = re.findall(r'\{ "(ring|amulet|staff|wand|rod|potion|mushroom)", "[^"]+", "[^"]+" \}', HEADER)
        self.assertEqual(len(types), 7)
        self.assertEqual(len(set(types)), 7)

    def test_visibility_gated_before_translation(self):
        self.assertIn('#include "zh-tw-flavored-object.h"', DESC)
        self.assertIn("show_flavor && obj->kind->flavor && !obj->artifact", DESC)
        self.assertIn("!obj->ego && obj->tval != TV_SCROLL", DESC)
        self.assertIn("if (aware && obj->kind->name) {", DESC)
        self.assertIn("if (!visible_effect) visible_effect = obj->kind->name;", DESC)
        self.assertIn("visible_effect = angdroid_zh_tw_item_kind_name(", DESC)
        self.assertIn("if (!aware || visible_effect)", DESC)
        self.assertIn("tval_find_name(obj->tval), appearance,", DESC)
        self.assertIn("localized_flavor : obj->kind->flavor->text;", DESC)
        self.assertIn("else if (localized_flavored)", DESC)
        self.assertIn("angdroid_zh_tw_item_append_whole(buf, max, &end,", DESC)
        self.assertIn("!localized_flavored &&", DESC)

    def test_fallback_export_rng_and_utf8_bounds(self):
        self.assertIn("obj_desc_name_format(buf, max, end, basename, modstr, plural)", DESC)
        self.assertIn("Rand_value = seed_flavor;", RNG)
        self.assertIn("if (needed >= capacity) return false;", HEADER)
        self.assertNotIn("zh-tw-flavored-object.h", EXPORT)
        self.assertNotIn("zh-tw-flavored-object.h", RNG)


if __name__ == "__main__":
    unittest.main()
