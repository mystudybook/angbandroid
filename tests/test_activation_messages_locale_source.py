#!/usr/bin/env python3
"""Activation and artifact runtime messages: 50 canonical zh-TW templates.

Data-driven coverage, English mode and save/gameplay invariants for the
original activation_message -> print_custom_message pipeline.
"""
from pathlib import Path
import re
import unittest

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp/angband"
DATA = ROOT / "variant_lib/angband/gamedata"
DICTIONARY = (CPP / "zh-tw-activation-messages.h").read_text(encoding="utf-8")
CMD = (CPP / "cmd-obj.c").read_text(encoding="utf-8")
PRINTER = (CPP / "obj-util.c").read_text(encoding="utf-8")
NATIVE = (ROOT / "tests/native/test_activation_message_locale.c").read_text(
    encoding="utf-8")
NATIVE_RUNNER = (ROOT / "scripts/test-native-utf8.sh").read_text(
    encoding="utf-8")


class ActivationMessagesLocaleSourceTest(unittest.TestCase):
    def test_exact_canonical_message_templates_with_alt_artifacts(self):
        sources = []
        for file in ("activation.txt", "artifact.txt"):
            content = (DATA / file).read_text(encoding="utf-8")
            sources.extend(re.findall(r"^msg:(.+)$", content, re.M))
        entries = re.findall(
            r'^\s*\{\s*"([^"\n]+)",\s*"([^"\n]+)"\s*\},\s*$',
            DICTIONARY, re.M)
        self.assertEqual(len(sources), 53)
        self.assertEqual(len(set(sources)), 50)
        self.assertEqual(len(entries), 50)
        self.assertEqual(len({key for key, _ in entries}), 50)
        self.assertEqual(set(sources), {key for key, _ in entries})
        self.assertEqual(
            sum(1 for text in (DATA / "artifact.txt").read_text(
                encoding="utf-8").splitlines() if text.startswith("msg:")), 5)
        for english, translated in entries:
            with self.subTest(english=english):
                self.assertTrue(translated)
                self.assertIn(
                    'check(' + __import__("json").dumps(
                        english, ensure_ascii=False) + ",", NATIVE)
                self.assertNotIn("{kind}", translated)
                self.assertNotIn("{s}", translated)
                self.assertNotIn("{is}", translated)
                if "{" in translated:
                    self.assertTrue(
                        translated.startswith("{name}"), translated)
                    self.assertEqual(translated.count("{name}"), 1)
                    self.assertEqual(translated.count("{"), 1)

    def test_activation_template_selected_after_original_alt_override(self):
        block = CMD.split("static void activation_message(", 1)[1].split(
            "\n\n\n/**\n * ------------------------------------------------------------------------\n * Inscriptions", 1)[0]
        self.assertIn('#include "zh-tw-activation-messages.h"', CMD)
        self.assertIn("if (!obj->activation) return;", block)
        self.assertIn("if (!obj->activation->message) return;", block)
        self.assertIn("if (obj->artifact && obj->artifact->alt_msg)", block)
        self.assertIn("message = obj->artifact->alt_msg;", block)
        self.assertIn("message = obj->activation->message;", block)
        self.assertLess(
            block.index("message = obj->activation->message;"),
            block.index("angdroid_zh_tw_activation_message("))
        self.assertIn("angdroid_zh_tw_enabled()", block)
        self.assertIn("print_custom_message(obj,", block)
        self.assertIn("MSG_GENERIC, p);", block)
        self.assertNotIn("obj->activation->message =", block)
        self.assertNotIn("obj->artifact->alt_msg =", block)

    def test_dynamic_names_localized_by_original_custom_formatter(self):
        block = PRINTER.split("void print_custom_message(", 1)[1].split(
            "\n\n/**\n * Return if the given artifact", 1)[0]
        self.assertIn("if (!string) return;", block)
        self.assertIn("switch(msg_tag_lookup(tag))", block)
        self.assertIn("case MSG_TAG_NAME:", block)
        self.assertIn("ODESC_PREFIX | ODESC_BASE |", block)
        self.assertIn(
            "(angdroid_zh_tw_enabled() ? ODESC_ZH_TW : 0), p);", block)
        self.assertIn("case MSG_TAG_KIND:", block)
        self.assertIn("case MSG_TAG_VERB:", block)
        self.assertIn("case MSG_TAG_VERB_IS:", block)
        self.assertIn("obj->number == 1", block)
        self.assertIn('msgt(msg_type, "%s", buf);', block)
        self.assertNotIn("ODESC_SPOIL", block)
        self.assertNotIn("obj->known->activation =", block)

    def test_original_activation_and_cancel_charge_policies_unchanged(self):
        use = CMD.split("static void use_aux(", 1)[1].split(
            "\n\n/**\n * Read a scroll", 1)[0]
        self.assertIn("if (obj->activation) {", use)
        self.assertIn("activation_message(obj, player);", use)
        self.assertIn('cmd_get_target(cmd, "target", &dir) != CMD_OK', use)
        self.assertLess(
            use.index('cmd_get_target(cmd, "target", &dir)'),
            use.index("can_use = check_devices(obj);"))
        self.assertIn("if (used && !deduct_before)", use)
        self.assertIn("if (!used) {", use)
        self.assertIn("if (deduct_before) {", use)
        self.assertIn("obj->pval = charges;", use)
        self.assertIn("obj->timeout = charges;", use)
        self.assertIn("used = effect_do(effect,", use)
        self.assertIn("player->upkeep->energy_use = z_info->move_energy;", use)
        self.assertIn("if (!obj_can_activate(obj)) {", CMD)
        self.assertIn("That item is still charging.", CMD)

    def test_native_bilingual_and_unknown_template_fallback_registered(self):
        self.assertIn("if (!english || !zh_tw) return english;", DICTIONARY)
        self.assertIn("strcmp(english, map[i].en) == 0", DICTIONARY)
        self.assertIn("return english;", DICTIONARY)
        self.assertIn("tests/native/test_activation_message_locale.c", NATIVE_RUNNER)
        self.assertIn(
            'angdroid_zh_tw_activation_message(unknown, 1) == unknown',
            NATIVE)
        self.assertIn("angdroid_zh_tw_activation_message(NULL, 1) == NULL",
                      NATIVE)


if __name__ == "__main__":
    unittest.main()
