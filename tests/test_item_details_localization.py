#!/usr/bin/env python3
"""Ensure Chinese combat and annotations preserve Angband knowledge checks."""
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp/angband"
OBJ = (CPP / "obj-desc.c").read_text(encoding="utf-8")
HEADER = (CPP / "zh-tw-item-details.h").read_text(encoding="utf-8")
INSCRIPTION = (CPP / "obj-desc-inscription-utf8.h").read_text(encoding="utf-8")
EXPORT = (CPP / "save-charoutput.c").read_text(encoding="utf-8")
RAND = (CPP / "obj-util.c").read_text(encoding="utf-8")


class ItemDetailsLocalizationTest(unittest.TestCase):
    def test_eight_combat_fields_and_original_knowledge_guards(self):
        for field in ("DICE", "MULTIPLIER", "ATTACK_PAIR", "HIT", "DAMAGE",
                      "ARMOR_PAIR", "ARMOR", "ARMOR_BONUS"):
            self.assertIn("ANGDROID_ITEM_" + field, HEADER)
            self.assertIn("ANGDROID_ITEM_" + field, OBJ)
        self.assertIn("bool zh_tw = (mode & ODESC_ZH_TW) && angdroid_zh_tw_enabled();", OBJ)
        self.assertIn("(!p || (p->obj_k->dd && p->obj_k->ds))", OBJ)
        self.assertIn("if (!((obj->notice & OBJ_NOTICE_ASSESSED) || spoil)) return end;", OBJ)
        self.assertIn("if (!p || p->obj_k->to_a)", OBJ)
        self.assertIn("object_to_hit(obj)", OBJ)
        self.assertIn("object_to_dam(obj)", OBJ)
        self.assertIn("object_to_ac(obj)", OBJ)
        self.assertIn('strnfcat(buf, max, &end, " (%+d,%+d)", to_h, to_d);', OBJ)
        self.assertIn('strnfcat(buf, max, &end, " [%d,%+d]", obj->ac, to_a);', OBJ)

    def test_modifier_uniqueness_and_hidden_statuses(self):
        self.assertIn("if (obj->modifiers[i] != 0)", OBJ)
        self.assertIn("if (mods[j] == obj->modifiers[i]) break;", OBJ)
        self.assertIn("obj_desc_mods(obj->known, buf, max, end, mode)", OBJ)
        self.assertIn("angdroid_zh_tw_item_modifiers_text(", OBJ)
        self.assertIn("angdroid_zh_tw_item_append_amount(", OBJ)
        self.assertIn("number_charging(obj)", OBJ)
        self.assertIn("object_flavor_is_aware(obj)", OBJ)
        self.assertIn('u[n++] = quark_str(obj->note);', OBJ)
        self.assertIn("if (zh_tw && !(i == 0 && obj->note))", OBJ)
        self.assertIn("angdroid_obj_desc_inscription_append(buf, max, &end,", OBJ)
        self.assertIn('const char *separator = zh_tw ? "、" : ", ";', INSCRIPTION)
        self.assertIn("out[used++] = '}';", INSCRIPTION)

    def test_complete_utf8_append_and_no_persistent_changes(self):
        self.assertIn("written = angdroid_zh_tw_item_append_utf8(out, cap, end, text);", HEADER)
        self.assertIn("out[old] = '\\0';", HEADER)
        self.assertIn("if (wanted >= cap - old) return false;", HEADER)
        self.assertIn('"（命中 %+d／傷害 %+d）"', HEADER)
        self.assertIn('"%s%+d"', HEADER)
        self.assertNotIn("zh-tw-item-details.h", EXPORT)
        self.assertNotIn("zh-tw-item-details.h", RAND)


if __name__ == "__main__":
    unittest.main()
