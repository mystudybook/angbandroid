#!/usr/bin/env python3
"""Scroll title localization is screen-only and never changes the flavor RNG."""
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp/angband"
DESC = (CPP / "obj-desc.c").read_text(encoding="utf-8")
SOURCE = (CPP / "obj-util.c").read_text(encoding="utf-8")
HEADER = (CPP / "zh-tw-scroll-title.h").read_text(encoding="utf-8")
EXPORT = (CPP / "save-charoutput.c").read_text(encoding="utf-8")
TEST_SCRIPT = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")


class ScrollTitleDisplayTests(unittest.TestCase):
    def test_randomizer_is_untouched_and_original_names_remain_identifiers(self):
        self.assertIn("Rand_value = seed_flavor;", SOURCE)
        self.assertIn("randname_make(RANDNAME_SCROLL,", SOURCE)
        self.assertIn("f->text = scroll_adj[k_info[i].sval];", SOURCE)
        self.assertIn("flavor_assign_random(TV_SCROLL);", SOURCE)
        self.assertNotIn("zh-tw-scroll-title.h", SOURCE)
        self.assertNotIn("ODESC_ZH_TW", SOURCE)
        self.assertNotIn("zh-tw-scroll-title.h", EXPORT)

    def test_screen_only_and_identification_aware(self):
        self.assertIn('#include "zh-tw-scroll-title.h"', DESC)
        self.assertIn("if (!easy_know && !kind->aware && kind->flavor)", DESC)
        self.assertIn("kind->tval == TV_SCROLL && angdroid_zh_tw_enabled()", DESC)
        self.assertIn("angdroid_zh_tw_scroll_title(buf, max, flavor, false)", DESC)
        self.assertIn("(mode & ODESC_ZH_TW) && angdroid_zh_tw_enabled()", DESC)
        self.assertIn("obj->tval == TV_SCROLL && show_flavor", DESC)
        self.assertIn("!obj->artifact && !obj->ego && obj->kind->flavor", DESC)
        self.assertIn("obj->kind->flavor->text, true", DESC)
        self.assertIn("else if (localized_scroll && aware && !obj->artifact)", DESC)
        self.assertIn("angdroid_zh_tw_item_kind_name(obj->kind->name)", DESC)
        self.assertIn("angdroid_obj_desc_append_scroll_effect(buf, max, &end,", DESC)
        self.assertIn("known ? known : obj->kind->name", DESC)
        self.assertNotIn('snprintf(effect_label, sizeof(effect_label),', DESC)
        self.assertNotIn('strnfcat(buf, max, &end, " of %s"', DESC)
        self.assertIn("else if (!localized_kind && aware", DESC)

    def test_title_is_opaque_with_bounded_c99_buffer(self):
        for token in ("canonical[0] != '\"'", "canonical[n - 1] != '\"'",
                      "left + letters + right + 1 > capacity", "memcpy(out",
                      "return false;", "return true;"):
            self.assertIn(token, HEADER)
        self.assertIn('const char *before = full ? "題為「" : "「";', HEADER)
        self.assertIn('const char *after = full ? "」的卷軸" : "」";', HEADER)
        self.assertIn("test_scroll_title_locale.c", TEST_SCRIPT)


if __name__ == "__main__":
    unittest.main()
