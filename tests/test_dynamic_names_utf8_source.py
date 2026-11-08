#!/usr/bin/env python3
"""Source wiring regression for display-only UTF-8 dynamic names."""
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp/angband"
MON = (CPP / "mon-desc.c").read_text(encoding="utf-8")
INFO = (CPP / "obj-info.c").read_text(encoding="utf-8")
DESC = (CPP / "obj-desc.c").read_text(encoding="utf-8")
RENDER = (CPP / "zh-tw-monster-list-render.h").read_text(encoding="utf-8")
RUNNER = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")
CHECKER = (ROOT / "scripts/check_native_game_localization.py").read_text(
    encoding="utf-8")


class DynamicNamesUtf8SourceTest(unittest.TestCase):
    def test_monster_list_preserves_original_english_branch(self):
        self.assertIn('#include "zh-tw-monster-list-render.h"', MON)
        self.assertIn("angdroid_zh_tw_monster_list_copy(buf, buflen,", MON)
        self.assertIn('strnfmt(buf, buflen, "[U] %s", race->name);', MON)
        self.assertIn('my_strcat(buf, race->plural, buflen);', MON)
        self.assertIn('angdroid_utf8_append_complete(', RENDER)
        self.assertIn('snprintf(out, capacity, "[U] ")', RENDER)
        self.assertIn('snprintf(out, capacity, "%3d ", number)', RENDER)
        self.assertNotIn('strnfmt(buf, buflen, "[U] %s", translated)', MON)

    def test_item_origin_unique_and_regular_fallback(self):
        self.assertIn('#include "zh-tw-unique-monsters.h"', INFO)
        self.assertIn('angdroid_zh_tw_unique_monster_name(dropper)', INFO)
        self.assertIn('angdroid_zh_tw_monster_list_name(dropper)', INFO)
        self.assertIn("if (found) zh_dropper = found;", INFO)
        self.assertIn('if (!obj->origin_race)', INFO)
        self.assertIn('if (terse && !obj_can_wear(obj))', INFO)
        self.assertIn('textblock_append(tb, origins[origin].desc, name, loot_spot)', INFO)

    def test_opt_in_object_name_appends_never_cut_han_bytes(self):
        self.assertIn("angdroid_zh_tw_item_append_utf8(buf, max, &end, scroll_display)", DESC)
        self.assertIn("angdroid_zh_tw_item_append_utf8(buf, max, &end, flavored_display)", DESC)
        self.assertIn("angdroid_zh_tw_item_append_utf8(buf, max, &end,\n\t\t\t\tlocalized_kind)", DESC)
        self.assertIn("angdroid_zh_tw_item_append_whole(", DESC)
        self.assertIn('strnfcat(buf, max, &end, " %s", artifact_name)', DESC)
        self.assertIn('strnfcat(buf, max, &end, " %s", ego_name)', DESC)
        self.assertIn('if ((mode & ODESC_ZH_TW) && angdroid_zh_tw_enabled()', DESC)

    def test_host_regressions_registered(self):
        self.assertIn('test_monster_list_render_utf8.c', RUNNER)
        self.assertIn('test_dynamic_names_utf8_source.py', (
            ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8"))
        self.assertIn('angdroid_zh_tw_monster_list_copy(buf, buflen,', CHECKER)


if __name__ == "__main__":
    unittest.main()
