#!/usr/bin/env python3
"""Display-only zh-TW shop titles and inscription verification source guards.

All canonical keys are matched to shipped Angband terrain and command data,
while the original native store, item, inscription and save rules remain.
"""
from pathlib import Path
import re
import unittest

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp/angband"
TERRAIN = (ROOT / "variant_lib/angband/gamedata/terrain.txt").read_text(encoding="utf-8")
CMDS = (CPP / "cmd-core.c").read_text(encoding="utf-8")
SHOPS = (CPP / "zh-tw-store-names.h").read_text(encoding="utf-8")
WARNINGS = (CPP / "zh-tw-inscription-confirm.h").read_text(encoding="utf-8")
STORE = (CPP / "ui-store.c").read_text(encoding="utf-8")
ITEM = (CPP / "ui-object.c").read_text(encoding="utf-8")
VERIFY = (CPP / "obj-util.c").read_text(encoding="utf-8")
SCRIPT = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")


def pairs(text):
    return re.findall(r'\{\s*"([^"]+)"\s*,\s*"([^"]+)"\s*\}', text)


class StoreAndInscriptionLocaleSourceTest(unittest.TestCase):
    def test_shop_names_match_original_terrain_without_data_changes(self):
        official_names = re.findall(
            r"(?m)^code:(?:STORE_[A-Z]+|HOME)\nname:([^\n]+)", TERRAIN)
        translated = pairs(SHOPS)
        self.assertEqual(8, len(official_names))
        self.assertEqual(8, len(translated))
        self.assertEqual(len(translated), len(set(k for k, _ in translated)))
        self.assertEqual(set(official_names), set(k for k, _ in translated))
        self.assertIn("if (!english || !zh_tw) return english;", SHOPS)
        self.assertIn("return english;", SHOPS)

    def test_shop_frame_only_localizes_name_for_onscreen_header(self):
        self.assertIn('#include "zh-tw-store-names.h"', STORE)
        frame = STORE.split("static void store_display_frame(", 1)[1].split(
            "static void store_display_help(", 1)[0]
        self.assertIn("angdroid_store_display_name(", frame)
        self.assertIn("f_info[store->feat].name, angdroid_zh_tw_enabled()", frame)
        self.assertIn('strnfmt(buf, sizeof(buf), "%s (%ld)", store_name,', frame)
        self.assertIn("(long)proprietor->max_cost", frame)
        self.assertIn("summary_cols + owner_cols + 4 <= (size_t)layout.width", frame)
        self.assertIn("store_put_clipped(COLOUR_WHITE, buf, owner_row, summary_col,", frame)
        for action in ("CMD_BUY", "CMD_SELL", "CMD_STASH", "CMD_RETRIEVE"):
            self.assertIn("cmdq_push(" + action + ");", STORE)

    def test_confirmation_uses_real_native_cmd_verbs_and_safe_fallback(self):
        official_verbs = re.findall(
            r'\{\s*CMD_[A-Z0-9_]+,\s*"([^"]+)"\s*,', CMDS)
        mapped = pairs(WARNINGS)
        self.assertEqual(21, len(mapped))
        self.assertEqual(len(mapped), len(set(k for k, _ in mapped)))
        self.assertEqual([], [k for k, _ in mapped if k not in official_verbs])
        self.assertIn('snprintf(out, cap, "確定要%s", translated)', WARNINGS)
        self.assertIn('snprintf(out, cap, "Really %s", verb)', WARNINGS)
        self.assertIn('if (!verb) verb = "do that with";', WARNINGS)
        self.assertIn("if (n < 0 || (size_t)n >= cap)", WARNINGS)

        gate = ITEM.split("bool get_item_allow(", 1)[1].split(
            "static bool get_tag(", 1)[0]
        self.assertIn("n = check_for_inscrip(obj, verify_inscrip);", gate)
        self.assertIn('n += check_for_inscrip(obj, "!*");', gate)
        self.assertIn("const char *verb = cmd_verb(cmd);", gate)
        self.assertIn("angdroid_inscription_confirm_prefix(prompt_buf,", gate)
        self.assertIn("angdroid_zh_tw_enabled()", gate)
        self.assertIn("while (n--) {", gate)
        self.assertIn("verify_object(prompt_buf, obj, player)", gate)
        self.assertIn("return false;", gate)
        self.assertIn("return (true);", gate)

    def test_verify_object_uses_display_translation_only(self):
        verification = VERIFY.split("bool verify_object(", 1)[1].split(
            "\ntypedef enum {", 1)[0]
        self.assertIn('#include "droid.h"', VERIFY)
        self.assertIn("ODESC_PREFIX | ODESC_FULL | ODESC_ZH_TW", verification)
        self.assertIn('strnfmt(out_val, sizeof(out_val), "%s %s? ",', verification)
        self.assertIn("return (get_check(out_val));", verification)
        self.assertIn("char o_name[256];", verification)
        self.assertIn("char out_val[384];", verification)
        self.assertIn("tests/native/test_store_inscription_labels.c", SCRIPT)


if __name__ == "__main__":
    unittest.main()
