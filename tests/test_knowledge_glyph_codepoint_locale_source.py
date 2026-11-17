#!/usr/bin/env python3
"""Source-level guard for knowledge glyph hex input and object/rune inscriptions.

Changing a native glyph must require a valid scalar and user confirmation;
cancellation, bad digits, and localizing a prompt must not mutate save data.
"""
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp/angband"
UI = (CPP / "ui-knowledge.c").read_text(encoding="utf-8")
DICT = (CPP / "zh-tw-ui.h").read_text(encoding="utf-8")
PARSER = (CPP / "knowledge-glyph-codepoint.h").read_text(encoding="utf-8")
NATIVE = (ROOT / "tests/native/test_knowledge_glyph_codepoint.c").read_text(
    encoding="utf-8")
RUN = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")


class KnowledgeGlyphCodepointSourceTest(unittest.TestCase):
    def test_native_glyph_picker_rejects_invalid_input(self):
        block = UI.rsplit("static bool glyph_command(", 1)[1].split(
            "\n\nstatic void display_group_member(", 1)[0]
        self.assertIn('#include "knowledge-glyph-codepoint.h"', UI)
        self.assertIn('char code_point[6] = "";', block)
        self.assertIn('"(up to 5 hex digits):", angdroid_zh_tw_enabled()', block)
        self.assertIn("code_point, sizeof(code_point)", block)
        self.assertIn("angdroid_knowledge_parse_glyph_hex(code_point,", block)
        self.assertIn("if (!angdroid_knowledge_parse_glyph_hex(", block)
        self.assertIn('"Invalid Unicode code point."', block)
        self.assertIn("*cur_char_ptr = (wchar_t)point;", block)
        self.assertLess(block.index("angdroid_knowledge_parse_glyph_hex"),
                        block.index("*cur_char_ptr = (wchar_t)point;"))
        self.assertNotIn("strtoul(code_point,", block)
        self.assertIn("case ESCAPE:", block)
        self.assertIn("*cur_char_ptr = char_old;", block)
        self.assertIn("if (get_string(", block)
        self.assertIn("return false;", block)

    def test_ascii_hex_scalar_parser_is_non_mutating_on_failure(self):
        self.assertIn("if (!input || !value) return false;", PARSER)
        self.assertIn("for (i = 0; i < 5 && input[i]; ++i)", PARSER)
        self.assertIn("if (i == 0 || input[i] != '\\0') return false;", PARSER)
        self.assertIn("codepoint >= 0xd800 && codepoint <= 0xdfff", PARSER)
        self.assertIn("codepoint < 0x20", PARSER)
        self.assertIn("codepoint >= 0x7f && codepoint <= 0x9f", PARSER)
        self.assertIn("codepoint >= 0xfdd0 && codepoint <= 0xfdef", PARSER)
        self.assertIn("(codepoint & 0xfffeu) == 0xfffeu", PARSER)
        self.assertIn("codepoint > 0x10ffffu", PARSER)
        self.assertLess(PARSER.index("if (i == 0 ||"),
                        PARSER.index("*value = codepoint;"))
        self.assertIn('check_hex("1F431", true, 0x1f431);', NATIVE)
        self.assertIn('check_hex("D800", false, 0);', NATIVE)
        self.assertIn('check_hex("1F44A0", false, 0);', NATIVE)

    def test_autoinscription_prompts_translated_without_changing_commands(self):
        self.assertEqual(2, UI.count('angdroid_game_ui_text("Inscribe with: ",'))
        self.assertIn('{"Inscribe with: ", "刻上文字："}', DICT)
        self.assertIn('{"(up to 5 hex digits):",', DICT)
        self.assertIn('{"Invalid Unicode code point.",', DICT)
        obj = UI.split("static void o_xtra_act(", 1)[1].split(
            "\n\n/**\n * Display known objects", 1)[0]
        rune = UI.split("static void rune_xtra_act(", 1)[1].split(
            "\n\n/**\n * Display rune knowledge.", 1)[0]
        self.assertIn("if (ch.code == '}')", obj)
        self.assertIn("remove_autoinscription(oid);", obj)
        self.assertIn("else if (ch.code == '{')", obj)
        self.assertIn("if (askfor_aux(text, sizeof(text), NULL))", obj)
        self.assertIn("add_autoinscription(oid, text, k->aware);", obj)
        self.assertIn("cmdq_push(CMD_AUTOINSCRIBE);", obj)
        self.assertIn("if (ch.code == '}')", rune)
        self.assertIn("rune_set_note(oid, NULL);", rune)
        self.assertIn("else if (ch.code == '{')", rune)
        self.assertIn("if (askfor_aux(note_text, sizeof(note_text), NULL))", rune)
        self.assertIn("rune_set_note(oid, note_text);", rune)
        self.assertIn("rune_autoinscribe(player, oid);", rune)
        self.assertIn("screen_save();", obj)
        self.assertIn("screen_load();", obj)
        self.assertIn("screen_save();", rune)
        self.assertIn("screen_load();", rune)

    def test_existing_inscriptions_reject_lossy_prefills(self):
        obj = UI.split("static void o_xtra_act(", 1)[1].split(
            "\n\n/**\n * Display known objects", 1)[0]
        rune = UI.split("static void rune_xtra_act(", 1)[1].split(
            "\n\n/**\n * Display rune knowledge.", 1)[0]
        self.assertIn('#include "knowledge-note-prefill.h"', UI)
        for block in (obj, rune):
            self.assertIn('angdroid_knowledge_note_prefill(', block)
            self.assertIn('knowledge_note_edit_unavailable();', block)
            self.assertIn('screen_load();', block)
        self.assertLess(obj.index('knowledge_note_edit_unavailable();'),
                        obj.index('if (askfor_aux(text, sizeof(text), NULL))'))
        self.assertLess(rune.index('knowledge_note_edit_unavailable();'),
                        rune.index('if (askfor_aux(note_text, sizeof(note_text), NULL))'))
        self.assertIn('get_autoinscription(k, k->aware)', obj)
        self.assertIn('quark_str(rune_note(oid))', rune)
        self.assertNotIn('strnfmt(text, sizeof(text), "%s",', obj)
        self.assertNotIn('strnfmt(note_text, sizeof(note_text), "%s",', rune)
        self.assertIn('tests/native/test_knowledge_note_prefill.c', RUN)
        self.assertIn('"$BUILD_DIR/test_knowledge_note_prefill"', RUN)

    def test_native_regression_registered(self):
        self.assertIn("tests/native/test_knowledge_glyph_codepoint.c", RUN)
        self.assertIn("test_knowledge_glyph_codepoint", RUN)
        self.assertIn('check_locale("Inscribe with: ", "刻上文字：");', NATIVE)
        self.assertIn("angdroid_game_ui_text(NULL, 1)", NATIVE)


if __name__ == "__main__":
    unittest.main()
