#!/usr/bin/env python3
"""All Angband object.txt msg/vis-msg entries are translated at render time.

The source invariant is important: only use feedback is localized, never
object definitions, activations, cancellation, targeting, charges or saves.
"""
from pathlib import Path
import re
import unittest

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp/angband"
DATA = (ROOT / "variant_lib/angband/gamedata/object.txt").read_text(
    encoding="utf-8")
HEADER = (CPP / "zh-tw-item-use-messages.h").read_text(encoding="utf-8")
COMMANDS = (CPP / "cmd-obj.c").read_text(encoding="utf-8")
NATIVE = (ROOT / "tests/native/test_item_use_effect_messages.c").read_text(
    encoding="utf-8")
RUNNER = (ROOT / "scripts/test-native-utf8.sh").read_text(
    encoding="utf-8")


class ObjectUseEffectMessageTest(unittest.TestCase):
    def test_exact_message_coverage_matches_original_object_txt(self):
        game_data = re.findall(r"^(?:msg|vis-msg):(.+)$", DATA, re.M)
        entries = re.findall(
            r'^\s*\{\s*"([^"\n]+)",\s*"([^"\n]+)"\s*\},\s*$',
            HEADER, re.M)
        english = [en for en, _ in entries]
        self.assertEqual(len(game_data), 20)
        self.assertEqual(len(set(game_data)), 16)
        self.assertEqual(len(entries), 16)
        self.assertEqual(len(set(english)), len(english))
        self.assertEqual(set(english), set(game_data))
        for en, zh in entries:
            with self.subTest(english=en):
                self.assertTrue(zh.strip())
                self.assertIn('mapped("' + en + '",', NATIVE)
        self.assertIn("vis-msg:A line of shimmering blue light appears.", DATA)
        self.assertIn('mapped("That tastes good.", "味道不錯。");', NATIVE)

    def test_rendering_only_translation_english_and_unknown_fallback(self):
        self.assertIn(
            "static inline const char *angdroid_zh_tw_item_use_message(",
            HEADER)
        self.assertIn("if (!english || !zh_tw) return english;", HEADER)
        self.assertIn("strcmp(english, map[i].en) == 0", HEADER)
        self.assertIn("return english;", HEADER)
        self.assertIn("tests/native/test_item_use_effect_messages.c", RUNNER)
        self.assertIn("angdroid_zh_tw_item_use_message(NULL, 1) == NULL",
                      NATIVE)
        self.assertIn("angdroid_zh_tw_item_use_message(unknown, 1) == unknown",
                      NATIVE)

    def test_main_item_effect_and_visual_branch_respect_visibility(self):
        use_aux = COMMANDS.split("static void use_aux(", 1)[1].split(
            "\n\n/**\n * Read a scroll", 1)[0]
        self.assertIn('#include "zh-tw-item-use-messages.h"', COMMANDS)
        self.assertIn("if (obj->activation)", use_aux)
        self.assertIn("activation_message(obj, player);", use_aux)
        self.assertIn("} else if (obj->kind->effect_msg) {", use_aux)
        self.assertIn(
            "obj->kind->effect_msg, angdroid_zh_tw_enabled()", use_aux)
        self.assertIn(
            "} else if (obj->kind->vis_msg && !player->timed[TMD_BLIND])",
            use_aux)
        self.assertIn(
            "obj->kind->vis_msg, angdroid_zh_tw_enabled()", use_aux)
        self.assertEqual(2, use_aux.count("angdroid_zh_tw_item_use_message("))
        self.assertIn('msgt(snd, "%s", angdroid_zh_tw_item_use_message(', use_aux)
        self.assertIn("sound(snd);", use_aux)
        self.assertNotIn("obj->kind->effect_msg =", use_aux)
        self.assertNotIn("obj->kind->vis_msg =", use_aux)

    def test_cancel_target_no_deduction_and_restore_on_unused(self):
        use_aux = COMMANDS.split("static void use_aux(", 1)[1].split(
            "\n\n/**\n * Read a scroll", 1)[0]
        self.assertIn("if (obj_needs_aim(obj)) {", use_aux)
        self.assertIn('cmd_get_target(cmd, "target", &dir) != CMD_OK', use_aux)
        self.assertIn("player_confuse_dir(player, &dir, false);", use_aux)
        self.assertLess(use_aux.index('cmd_get_target(cmd, "target", &dir)'),
                        use_aux.index("can_use = check_devices(obj);"))
        self.assertLess(use_aux.index("can_use = check_devices(obj);"),
                        use_aux.index("if (can_use) {"))
        self.assertIn("if (from_floor)", use_aux)
        self.assertIn("obj->pval--;", use_aux)
        self.assertIn("obj->timeout += randcalc(obj->time", use_aux)
        self.assertIn("used = effect_do(effect,", use_aux)
        self.assertIn("if (!used) {", use_aux)
        self.assertIn("if (deduct_before) {", use_aux)
        self.assertIn("obj->pval = charges;", use_aux)
        self.assertIn("obj->timeout = charges;", use_aux)
        self.assertIn("floor_object_for_use(player, obj, 1,", use_aux)
        self.assertIn("drop_near(cave, &wcopy, 0,", use_aux)
        self.assertIn("if (was_aware || !ident) {", use_aux)
        self.assertLess(use_aux.index("if (!used) {"),
                        use_aux.index("player->upkeep->energy_use ="))
        self.assertIn("if (used && !deduct_before)", use_aux)
        self.assertIn("player->upkeep->energy_use = z_info->move_energy;", use_aux)
        self.assertIn("object_learn_on_use(player, work_obj);", use_aux)
        self.assertNotIn("ODESC_SPOIL", use_aux)

    def test_existing_device_checks_and_charging_are_unchanged(self):
        for kind, predicate, failure, use_mode in (
            ("do_cmd_use_staff", "obj_has_charges(obj)",
             "That staff has no charges.", "USE_CHARGE"),
            ("do_cmd_aim_wand", "obj_has_charges(obj)",
             "That wand has no charges.", "USE_CHARGE"),
            ("do_cmd_zap_rod", "obj_can_zap(obj)",
             "That rod is still charging.", "USE_TIMEOUT"),
            ("do_cmd_activate", "obj_can_activate(obj)",
             "That item is still charging.", "USE_TIMEOUT"),
        ):
            block = COMMANDS.split("void " + kind + "(", 1)[1].split(
                "\n}\n", 1)[0]
            with self.subTest(kind=kind):
                self.assertIn(predicate, block)
                self.assertIn(failure, block)
                self.assertIn("use_aux(cmd, obj, " + use_mode, block)
                self.assertIn("if (!player_get_resume_normal_shape", block)
        self.assertNotIn("zh-tw-item-use-messages.h",
                         (CPP / "obj-desc.c").read_text(encoding="utf-8"))


if __name__ == "__main__":
    unittest.main()
