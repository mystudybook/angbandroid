#!/usr/bin/env python3
"""Guard display-only localized item names and canonical save/export behavior."""
from __future__ import annotations

import re
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp/angband"
HEADER = (CPP / "obj-desc.h").read_text(encoding="utf-8")
DESCRIBE = (CPP / "obj-desc.c").read_text(encoding="utf-8")

UI_CALL_COUNTS = {
    "ui-object.c": 6,
    "ui-knowledge.c": 3,
    "store.c": 4,
}


class VisibleObjectLocalizationTest(unittest.TestCase):
    def test_explicit_nonpersistent_display_flag(self):
        self.assertRegex(HEADER, r"\bODESC_ZH_TW\s*=\s*0x800\b")
        self.assertIn("(mode & ODESC_ZH_TW) && angdroid_zh_tw_enabled()", DESCRIBE)
        self.assertIn("&& aware &&", DESCRIBE)
        self.assertIn("!show_flavor && obj->kind->name", DESCRIBE)
        self.assertIn("angdroid_zh_tw_item_kind_name(obj->kind->name)", DESCRIBE)
        self.assertIn("if (localized_kind)", DESCRIBE)
        self.assertIn("angdroid_zh_tw_known_item_name(", DESCRIBE)
        self.assertIn("angdroid_zh_tw_item_append_utf8(buf, max, &end,", DESCRIBE)
        self.assertIn("if (prefix && number == 0)", DESCRIBE)
        self.assertIn("else if (prefix && number > 1)", DESCRIBE)
        self.assertIn("!localized_kind && aware", DESCRIBE)
        # The original English grammar remains available to every unflagged
        # caller (including canonical name generation).
        self.assertIn("obj_desc_name_prefix(buf, max, end, obj, basename", DESCRIBE)
        self.assertIn("obj_desc_name_format(buf, max, end, basename, modstr, plural)", DESCRIBE)

    def test_ego_suffixes_only_display_when_identified(self):
        ego_header = (CPP / "zh-tw-ego-names.h").read_text(encoding="utf-8")
        pairs = re.findall(
            r'\{\s*"([^"\n]+)",\s*"([^"\n]+)"\s*\}', ego_header
        )
        self.assertEqual(len(pairs), 79)
        self.assertEqual(len({name for name, _ in pairs}), 79)
        self.assertIn('#include "zh-tw-ego-names.h"', DESCRIBE)
        self.assertIn('obj->known->ego && !(mode & ODESC_NOEGO)', DESCRIBE)
        self.assertIn('obj->ego && store', DESCRIBE)
        self.assertIn('const char *ego_name = obj->ego->name;', DESCRIBE)
        self.assertIn('angdroid_zh_tw_ego_name(ego_name)', DESCRIBE)
        self.assertIn('if (localized) ego_name = localized;', DESCRIBE)
        self.assertIn("angdroid_obj_desc_append_suffix_atomic(buf, max,", DESCRIBE)
        self.assertIn("&end, ego_name, true);", DESCRIBE)
        self.assertIn("&end, artifact_name, true);", DESCRIBE)
        self.assertNotIn('strnfcat(buf, max, &end, " %s", ego_name);', DESCRIBE)
        self.assertNotIn('strnfcat(buf, max, &end, " %s", artifact_name);', DESCRIBE)

    def test_artifact_names_only_after_identification(self):
        art_header = (CPP / "zh-tw-artifact-names.h").read_text(encoding="utf-8")
        pairs = re.findall(
            r'\{\s*"([^"\n]+)",\s*"([^"\n]+)"\s*\}', art_header
        )
        self.assertEqual(len(pairs), 138)
        self.assertEqual(len({name for name, _ in pairs}), 138)
        self.assertIn('#include "zh-tw-artifact-names.h"', DESCRIBE)
        self.assertIn("if (object_is_known_artifact(obj))", DESCRIBE)
        self.assertIn("const char *artifact_name = obj->artifact->name;", DESCRIBE)
        self.assertIn("angdroid_zh_tw_artifact_name(artifact_name)", DESCRIBE)
        self.assertIn("if (localized) artifact_name = localized;", DESCRIBE)
        self.assertIn("&end, artifact_name, true);", DESCRIBE)

    def test_unknown_item_flavor_only_changes_visible_appearance(self):
        flavor_header = (CPP / "zh-tw-flavor-names.h").read_text(encoding="utf-8")
        pairs = re.findall(
            r'\{\s*"([^"\n]+)",\s*"([^"\n]+)"\s*\}', flavor_header
        )
        self.assertEqual(len(pairs), 202)
        self.assertEqual(len({name for name, _ in pairs}), 202)
        self.assertIn('#include "zh-tw-flavor-names.h"', DESCRIBE)
        self.assertIn("if (!easy_know && !kind->aware && kind->flavor)", DESCRIBE)
        self.assertIn("angdroid_zh_tw_flavor_name(flavor)", DESCRIBE)
        self.assertIn("angdroid_utf8_copy_complete(buf, max, localized);", DESCRIBE)
        self.assertIn("my_strcpy(buf, flavor, max);", DESCRIBE)
        self.assertIn("angdroid_utf8_copy_complete(buf, max, translated);", DESCRIBE)
        self.assertNotIn("my_strcpy(buf, localized ? localized : flavor, max);", DESCRIBE)
        self.assertIn("show_flavor && obj->kind->flavor", DESCRIBE)
        self.assertIn("angdroid_zh_tw_flavor_name(obj->kind->flavor->text)", DESCRIBE)
        self.assertIn("if (localized_flavor) modstr = localized_flavor;", DESCRIBE)
        self.assertIn("const char *localized_kind = NULL;", DESCRIBE)

    def test_only_requested_screen_calls_opt_in(self):
        for name, expected in UI_CALL_COUNTS.items():
            content = (CPP / name).read_text(encoding="utf-8")
            calls = re.findall(r"\bobject_desc\((.*?)\);", content, re.S)
            self.assertEqual(len(calls), expected, name)
            self.assertTrue(all("ODESC_ZH_TW" in call for call in calls), name)
        # Character-output code is not opted in; mode is not globally toggled.
        export = (CPP / "save-charoutput.c").read_text(encoding="utf-8")
        self.assertNotIn("ODESC_ZH_TW", export)

    def test_user_notes_and_unknown_names_remain_safe(self):
        self.assertIn("if (zh_tw && !(i == 0 && obj->note))", DESCRIBE)
        self.assertIn("angdroid_obj_desc_inscription_append(buf, max, &end,", DESCRIBE)
        self.assertNotIn('strnfcat(buf, max, &end, "%s", u[i]);', DESCRIBE)
        self.assertIn('strnfmt(buf, max, "未知物品")', DESCRIBE)
        self.assertIn('strnfmt(buf, max, "an unknown item")', DESCRIBE)
        self.assertIn('"（剩餘 ", obj->pval, " 次充能）"', DESCRIBE)
        self.assertIn('"（充能中）"', DESCRIBE)
        self.assertIn('"（剩餘 ", obj->timeout, " 回合）"', DESCRIBE)
        self.assertIn('" (%d turns)"', DESCRIBE)

    def test_item_kind_mapping_is_still_complete(self):
        translated = (CPP / "zh-tw-items.h").read_text(encoding="utf-8")
        entries = re.findall(r'\{\s*"([^"\n]+)",\s*"([^"\n]+)"\s*\}', translated)
        self.assertEqual(len(entries), 326)
        self.assertEqual(len({english for english, _ in entries}), 326)


if __name__ == "__main__":
    unittest.main()
