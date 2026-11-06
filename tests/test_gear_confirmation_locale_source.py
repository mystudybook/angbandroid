#!/usr/bin/env python3
"""Traditional Chinese gear actions and Unicode-safe yes/no confirmation UI.

The display may translate names and instructions, but confirmation letters,
actual item selectors, wield-slot rules, and game data must not change.
"""
from pathlib import Path
import re
import unittest

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp/angband"
GEAR = (CPP / "cmd-obj.c").read_text(encoding="utf-8")
INPUT = (CPP / "ui-input.c").read_text(encoding="utf-8")
DICTIONARY = (CPP / "zh-tw-ui.h").read_text(encoding="utf-8")
HELPER = (CPP / "confirmation-prompt-display.h").read_text(encoding="utf-8")
NATIVE = (ROOT / "tests/native/test_gear_confirmation_locale.c").read_text(
    encoding="utf-8")
GAME_TEST = (ROOT / "tests/native/test_game_ui_locale.c").read_text(
    encoding="utf-8")
RUNNER = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")


class GearConfirmationLocaleSourceTest(unittest.TestCase):
    def test_wear_and_takeoff_prompts_are_bilingual_with_native_filters(self):
        takeoff = GEAR.split("void do_cmd_takeoff(", 1)[1].split(
            "\n\n/**\n * Wield or wear an item", 1)[0]
        wield = GEAR.split("void do_cmd_wield(", 1)[1].split(
            "\n\n/**\n * Drop an item", 1)[0]
        for english in (
            "Take off or unwield which item?",
            "You have nothing to take off or unwield.",
        ):
            self.assertIn('angdroid_game_ui_text("' + english + '",', takeoff)
        for english in (
            "Wear or wield which item?",
            "You have nothing to wear or wield.",
            "Really take off %s? ",
        ):
            self.assertIn('"' + english + '", angdroid_zh_tw_enabled()', wield)
        self.assertIn("obj_can_takeoff,", takeoff)
        self.assertIn("USE_EQUIP)", takeoff)
        self.assertIn("inven_takeoff(obj);", takeoff)
        self.assertIn("pack_overflow(obj);", takeoff)
        self.assertIn("obj_can_wear,", wield)
        self.assertIn("USE_INVEN | USE_FLOOR | USE_QUIVER)", wield)
        self.assertIn("slot = wield_slot(obj);", wield)
        self.assertIn("tval_is_ring(obj)", wield)
        self.assertIn("obj_can_takeoff(equip_obj)", wield)
        self.assertIn('n = check_for_inscrip(equip_obj, "!t");', wield)
        self.assertIn("while (n--)", wield)
        self.assertIn("if (!get_check(format(", wield)
        self.assertIn("inven_wield(obj, slot);", wield)
        self.assertIn("gear_to_label(player, equip_obj)", wield)
        self.assertIn("char o_name[256];", wield)
        self.assertIn("ODESC_BASE | ODESC_ZH_TW", wield)
        self.assertEqual(2, wield.count("ODESC_PREFIX | ODESC_FULL | ODESC_ZH_TW"))
        self.assertNotIn("ODESC_SPOIL", wield)

    def test_inscription_shows_known_name_without_changing_note_data(self):
        inscribe = GEAR.split("void do_cmd_inscribe(", 1)[1].split(
            "\n\n/**\n * Autoinscribe all appropriate objects", 1)[0]
        self.assertIn("char o_name[256];", inscribe)
        self.assertIn("ODESC_PREFIX | ODESC_FULL | ODESC_ZH_TW", inscribe)
        self.assertIn('angdroid_game_ui_text("Inscribing %s.",', inscribe)
        self.assertIn('"Inscribe with what? "', inscribe)
        self.assertIn("cmd_get_string(cmd, \"inscription\", &str", inscribe)
        self.assertIn("obj->note = quark_add(str);", inscribe)
        self.assertIn("PN_COMBINE | PN_IGNORE", inscribe)
        self.assertIn("PR_INVEN | PR_EQUIP", inscribe)
        self.assertNotIn("ODESC_SPOIL", inscribe)

    def test_confirm_and_choice_prompt_clip_utf8_not_bytes(self):
        check = INPUT.split("static bool textui_get_check(", 1)[1].split(
            "\n/* TODO: refactor get_check()", 1)[0]
        choose = INPUT.split("char get_char(", 1)[1].split(
            "\n\n/**", 1)[0]
        self.assertIn('#include "confirmation-prompt-display.h"', INPUT)
        self.assertIn("char buf[256];", check)
        self.assertIn("char buf[256];", choose)
        self.assertIn("char suffix[96];", choose)
        self.assertIn('angdroid_confirmation_prompt_display(buf, sizeof(buf), prompt,', check)
        self.assertIn('"[y/n] ", Term && Term->wid > 0 ?', check)
        self.assertIn('strnfmt(suffix, sizeof(suffix), "[%s] ", options)', choose)
        self.assertIn("angdroid_confirmation_prompt_display(buf, sizeof(buf), prompt,", choose)
        self.assertNotIn('%.70s[y/n]', check)
        self.assertNotIn('%.70s[%s]', choose)
        self.assertIn("ke = inkey_m();", check)
        self.assertIn("ke.type == EVT_MOUSE", check)
        self.assertIn("ke.key.code != 'Y'", check)
        self.assertIn("ke.key.code != 'y'", check)
        self.assertIn("return (true);", check)
        self.assertIn("key = inkey();", choose)
        self.assertIn("strchr(options, (char)key.code)", choose)
        self.assertIn("key.code = fallback;", choose)
        self.assertIn("return key.code;", choose)

    def test_helper_preserves_suffix_before_cutting_cjk(self):
        self.assertIn('#include "menu-text-fit.h"', HELPER)
        self.assertIn("suffix_cols = angdroid_utf8_display_columns(suffix);", HELPER)
        self.assertIn("angdroid_menu_copy_columns(", HELPER)
        self.assertIn("terminal_columns - suffix_cols", HELPER)
        self.assertIn("if (available > 70) available = 70;", HELPER)
        self.assertIn("prefix_capacity = suffix_bytes < capacity", HELPER)
        self.assertIn("capacity - message_bytes, suffix", HELPER)
        self.assertIn("if (!out || capacity == 0) return 0;", HELPER)
        self.assertIn("if (!suffix || terminal_columns == 0) return 0;", HELPER)
        self.assertIn("tests/native/test_gear_confirmation_locale.c", RUNNER)
        self.assertIn("build(\"abc\\xE4\\xB8\"", NATIVE)
        self.assertIn("for (capacity = 1; capacity <= 96; ++capacity)", NATIVE)

    def test_all_six_translation_keys_have_real_bilingual_regressions(self):
        pairs = {
            "Wear or wield which item?": "要穿戴或裝備哪件物品？",
            "You have nothing to wear or wield.": "沒有可以穿戴或裝備的物品。",
            "Take off or unwield which item?": "要卸下哪件裝備？",
            "You have nothing to take off or unwield.": "沒有可以卸下的裝備。",
            "Really take off %s? ": "確定卸下 %s 嗎？ ",
            "Inscribing %s.": "正在為 %s 刻字。",
        }
        existing = re.findall(r'\{\s*"([^"\n]+)"\s*,\s*"([^"\n]+)"\s*\}', DICTIONARY)
        self.assertEqual(len(existing), len(dict(existing)), "duplicate UI text keys")
        dictionary = dict(existing)
        for english, translated in pairs.items():
            self.assertEqual(dictionary.get(english), translated)
            self.assertIn('angdroid_game_ui_text("' + english + '", 1)', GAME_TEST)
            self.assertIn('angdroid_game_ui_text("' + english + '", 0)', GAME_TEST)
        self.assertIn("angdroid_game_ui_text(NULL, 1) == NULL", GAME_TEST)


if __name__ == "__main__":
    unittest.main()
