#!/usr/bin/env python3
"""Angband item-use prompts and remaining-item feedback: display-only zh-TW.

All command argument names, filter predicates, allowed item sources, stack
arithmetic, turn energy and the original object knowledge gate are preserved.
"""
from pathlib import Path
import re
import unittest

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp/angband"
COMMANDS = (CPP / "cmd-obj.c").read_text(encoding="utf-8")
UI = (CPP / "zh-tw-ui.h").read_text(encoding="utf-8")
NATIVE = (ROOT / "tests/native/test_item_use_locale.c").read_text(
    encoding="utf-8")
NATIVE_RUNNER = (ROOT / "scripts/test-native-utf8.sh").read_text(
    encoding="utf-8")


def command(name, next_name):
    return COMMANDS.split("void " + name + "(", 1)[1].split(
        "\nvoid " + next_name + "(", 1)[0]


class ItemUsePromptsLocaleSourceTest(unittest.TestCase):
    def test_selector_prompts_and_filters_unchanged(self):
        scenarios = [
            ("do_cmd_read_scroll", "do_cmd_use_staff",
             "Read which scroll? ", "You have no scrolls to read.",
             "tval_is_scroll,", "USE_INVEN | USE_FLOOR"),
            ("do_cmd_eat_food", "do_cmd_quaff_potion",
             "Eat which food? ", "You have no food to eat.",
             "tval_is_edible,", "USE_INVEN | USE_FLOOR"),
            ("do_cmd_quaff_potion", "do_cmd_use",
             "Quaff which potion? ", "You have no potions from which to quaff.",
             "tval_is_potion,", "USE_INVEN | USE_FLOOR"),
            ("do_cmd_use", "do_cmd_refill",
             "Use which item? ", "You have no items to use.",
             "obj_is_useable,", "USE_EQUIP | USE_INVEN | USE_QUIVER | USE_FLOOR"),
        ]
        for name, next_name, prompt, empty, filter_call, sources in scenarios:
            with self.subTest(command=name):
                section = command(name, next_name)
                self.assertIn('cmd_get_item(cmd, "item", &obj,', section)
                for text in (prompt, empty):
                    self.assertIn(
                        'angdroid_game_ui_text("' + text +
                        '", angdroid_zh_tw_enabled())', section)
                self.assertIn(filter_call, section)
                self.assertIn(sources, section)
                self.assertIn("!= CMD_OK) return;", section)

    def test_one_time_item_consumption_and_dispatch_not_modified(self):
        read = command("do_cmd_read_scroll", "do_cmd_use_staff")
        food = command("do_cmd_eat_food", "do_cmd_quaff_potion")
        potion = command("do_cmd_quaff_potion", "do_cmd_use")
        self.assertIn("if (!player_can_read(player, true))", read)
        self.assertIn("use_aux(cmd, obj, USE_SINGLE, MSG_GENERIC);", read)
        self.assertIn("use_aux(cmd, obj, USE_SINGLE, MSG_EAT);", food)
        self.assertIn("use_aux(cmd, obj, USE_SINGLE, MSG_QUAFF);", potion)
        general = command("do_cmd_use", "do_cmd_refill")
        for text in (
            "do_cmd_fire(cmd)", "do_cmd_quaff_potion(cmd)",
            "do_cmd_eat_food(cmd)", "do_cmd_zap_rod(cmd)",
            "do_cmd_aim_wand(cmd)", "do_cmd_use_staff(cmd)",
            "do_cmd_read_scroll(cmd)", "do_cmd_refill(cmd)",
            "do_cmd_activate(cmd)",
        ):
            self.assertIn(text, general)
        self.assertIn("object_is_equipped(player->body, obj)", general)
        self.assertIn("obj_is_useable,", general)
        self.assertIn("QUIVER_TAGS", general)

    def test_refuel_wording_corrected_without_changing_fuel(self):
        refill = COMMANDS.split("void do_cmd_refill(", 1)[1].split(
            "\n\n/**\n * ------------------------------------------------------------------------\n * Spell casting", 1)[0]
        self.assertIn(
            'angdroid_game_ui_text("Refuel with which fuel source? ", '
            'angdroid_zh_tw_enabled())', refill)
        self.assertNotIn("Refuel with with fuel source?", refill)
        self.assertIn("obj_can_refill,", refill)
        self.assertIn("USE_INVEN | USE_FLOOR | USE_QUIVER", refill)
        self.assertIn('equipped_item_by_slot_name(player, "light")', refill)
        self.assertIn("if (!light || !tval_is_light(light))", refill)
        self.assertIn("OF_NO_FUEL", refill)
        self.assertIn("OF_TAKES_FUEL", refill)
        self.assertIn("refill_lamp(light, obj);", refill)
        self.assertIn("z_info->move_energy / 2", refill)

    def test_remaining_item_message_uses_known_chinese_name_only(self):
        use_aux = COMMANDS.split("static void use_aux(", 1)[1].split(
            "\n\n/**\n * Read a scroll", 1)[0]
        self.assertIn("char name[256];", use_aux)
        self.assertIn("ODESC_PREFIX | ODESC_FULL | ODESC_ALTNUM |", use_aux)
        self.assertIn("ODESC_ZH_TW |", use_aux)
        self.assertIn("((number + ((used && use == USE_SINGLE) ?", use_aux)
        self.assertIn("-1 : 0)) << 16), player);", use_aux)
        self.assertNotIn("ODESC_SPOIL", use_aux)
        self.assertIn("if (from_floor)", use_aux)
        self.assertIn('msg("你看見 %s。", name);', use_aux)
        self.assertIn('msg("你持有 %s（第一組 %c）。", name, label);', use_aux)
        self.assertIn('msg("你持有 %s（%c）。", name, label);', use_aux)
        self.assertIn("gear_to_label(player, first_remainder)", use_aux)
        self.assertIn("floor_item_charges(work_obj);", use_aux)
        self.assertIn("inven_item_charges(work_obj);", use_aux)
        self.assertIn("object_delete(cave, player->cave, &work_obj);", use_aux)
        self.assertIn("player->upkeep->energy_use = z_info->move_energy;", use_aux)

    def test_exact_translation_pairs_and_host_test_registration(self):
        expected = {
            "Read which scroll? ": "要閱讀哪張卷軸？ ",
            "You have no scrolls to read.": "你沒有可以閱讀的卷軸。",
            "Eat which food? ": "要吃哪種食物？ ",
            "You have no food to eat.": "你沒有可以吃的食物。",
            "Quaff which potion? ": "要喝哪瓶藥水？ ",
            "You have no potions from which to quaff.": "你沒有可以喝的藥水。",
            "Use which item? ": "要使用哪件物品？ ",
            "You have no items to use.": "你沒有可使用的物品。",
            "Refuel with which fuel source? ": "要使用哪種燃料補充光源？ ",
        }
        pairs = re.findall(
            r'\{\s*"([^"\n]+)"\s*,\s*"([^"\n]+)"\s*\}', UI)
        self.assertEqual(len(pairs), len({eng for eng, _ in pairs}))
        dictionary = dict(pairs)
        for en, zh in expected.items():
            with self.subTest(en=en):
                self.assertEqual(dictionary.get(en), zh)
                self.assertIn('mapped("' + en + '"', NATIVE)
        self.assertNotIn('"Refuel with with fuel source? "', UI)
        self.assertIn("tests/native/test_item_use_locale.c", NATIVE_RUNNER)
        self.assertIn("angdroid_game_ui_text(NULL, 1) == NULL", NATIVE)


if __name__ == "__main__":
    unittest.main()
