#!/usr/bin/env python3
"""Source wiring regression for display-only UTF-8 dynamic names."""
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp/angband"
MON = (CPP / "mon-desc.c").read_text(encoding="utf-8")
LORE = (CPP / "ui-mon-lore.c").read_text(encoding="utf-8")
INFO = (CPP / "obj-info.c").read_text(encoding="utf-8")
DESC = (CPP / "obj-desc.c").read_text(encoding="utf-8")
RENDER = (CPP / "zh-tw-monster-list-render.h").read_text(encoding="utf-8")
KNOWLEDGE = (CPP / "ui-knowledge.c").read_text(encoding="utf-8")
STORE = (CPP / "zh-tw-knowledge-menu.h").read_text(encoding="utf-8")
STORE_TEST = (ROOT / "tests/native/test_knowledge_store_menu_locale.c").read_text(
    encoding="utf-8")
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

    def test_monster_recall_title_uses_known_unique_translation(self):
        self.assertIn('#include "zh-tw-unique-monsters.h"', LORE)
        self.assertIn('angdroid_zh_tw_unique_monster_name(race->name)', LORE)
        self.assertIn('angdroid_zh_tw_monster_list_name(race->name)', LORE)
        self.assertIn('if (!name_zh) textblock_append(tb, "The ");', LORE)
        self.assertIn('else if (OPT(player, purple_uniques))', LORE)
        self.assertIn('name_zh ? name_zh : race->name', LORE)
        self.assertIn('if (!spoilers)', LORE)

    def test_object_kind_name_copies_complete_translated_characters(self):
        self.assertIn('#include "../curses/unicode_display.h"', DESC)
        self.assertIn('angdroid_utf8_copy_complete(buf, max, localized);', DESC)
        self.assertIn('angdroid_utf8_copy_complete(buf, max, translated);', DESC)
        self.assertIn('my_strcpy(buf, flavor, max);', DESC)
        self.assertIn('obj_desc_name_format(buf, max, 0, kind->name, NULL, false)', DESC)
        self.assertIn('if (!easy_know && !kind->aware && kind->flavor)', DESC)
        self.assertIn('test_item_kind_utf8_copy.c', RUNNER)
        self.assertIn('"$BUILD_DIR/test_item_locale"', RUNNER)

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
        self.assertIn('angdroid_zh_tw_item_append_amount(buf, max, &end,', DESC)
        self.assertNotIn('strnfcat(buf, max, &end, "%u 張", number)', DESC)
        self.assertNotIn('strnfcat(buf, max, &end, "沒有剩餘的")', DESC)
        self.assertIn('strnfcat(buf, max, &end, " %s", artifact_name)', DESC)
        self.assertIn('strnfcat(buf, max, &end, " %s", ego_name)', DESC)
        self.assertIn('if ((mode & ODESC_ZH_TW) && angdroid_zh_tw_enabled()', DESC)

    def test_long_store_names_keep_chinese_fallback_and_shortcut(self):
        self.assertIn('angdroid_knowledge_store_label_for_menu(localized,',
                      KNOWLEDGE)
        self.assertIn('static inline const char *angdroid_knowledge_store_label_for_menu(',
                      STORE)
        self.assertIn('NULL, store_number, shortcut, zh_tw)', STORE)
        self.assertIn('"查看第 11 間商店的商品 (9)"', STORE_TEST)
        self.assertIn('long_name, 11, " (9)", 0)', STORE_TEST)
        self.assertIn('main_knowledge_menu.actions[i].action =',
                      KNOWLEDGE)

    def test_host_regressions_registered(self):
        self.assertIn('test_monster_list_render_utf8.c', RUNNER)
        self.assertIn('test_dynamic_names_utf8_source.py', (
            ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8"))
        self.assertIn('angdroid_zh_tw_monster_list_copy(buf, buflen,', CHECKER)


if __name__ == "__main__":
    unittest.main()
