#!/usr/bin/env python3
"""FAangband source-level contract for typed race/class ability labels.

Canonical flag indexes identify the concept to translate. The original
English data, saves, filters and command IDs must remain unchanged.
"""
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[1]
FA = ROOT / "app/src/main/cpp/faangband"
BIRTH = (FA / "ui-birth.c").read_text(encoding="utf-8")
TRAITS = (FA / "ui-birth-abilities-zh-tw.h").read_text(encoding="utf-8")
PF = (FA / "list-player-flags.h").read_text(encoding="utf-8")
OF = (FA / "list-object-flags.h").read_text(encoding="utf-8")
RUNNER = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")


def require(source, *terms):
    for term in terms:
        assert term in source, term


player_flags = {
    "PF_" + name for name in re.findall(r"\bPF\((\w+)\)", PF)
}
object_flags = {
    "OF_" + name for name in re.findall(r"\bOF\((\w+),", OF)
}
translated_player = re.findall(r"case (PF_\w+): return \"([^\"]+)\";", TRAITS)
translated_object = re.findall(r"case (OF_\w+): return \"([^\"]+)\";", TRAITS)
assert len(translated_player) >= 65
assert len(translated_object) >= 20

for pairs, known in (
    (translated_player, player_flags),
    (translated_object, object_flags),
):
    names = [name for name, _ in pairs]
    assert len(names) == len(set(names)), "duplicate switch labels"
    assert set(names) <= known, "unknown flag would fail C compilation"
    assert all(text.strip() and all(ord(c) < 128 or ord(c) >= 0x3000
                                    for c in text) for _, text in pairs)
    assert all("\x00" not in text for _, text in pairs)

require(TRAITS,
    "static inline const char *angdroid_fa_birth_ability_name(",
    "const char *english, const char *type, int index, int zh_tw)",
    'if (!english || !zh_tw || !type) return english;',
    'if (strcmp(type, "player") == 0)',
    '} else if (strcmp(type, "object") == 0)',
    'return angdroid_game_ui_text(english, zh_tw);',
    'case PF_HARDY: return "堅韌體質";',
    'case PF_SHIELD_BASH: return "盾擊";',
    'case PF_ARMSMAN: return "武器專家";',
    'case OF_TELEPATHY: return "心靈感應";',
    'case OF_FREE_ACT: return "行動自由";',
)
for bad in ("cmdq_push(", "cmd_set_arg_", "pf_on(", "pf_off(",
            "of_on(", "of_off(", "player->", "savefile"):
    assert bad not in TRAITS, bad

# All birth-sheet variants reuse the same typed translator. Its caller
# still filters traits by the original ability type/index/value flags.
require(BIRTH,
    '#include "ui-birth-abilities-zh-tw.h"',
    "ability->name, ability->type, ability->index, zh));",
    "ability->name, ability->type, ability->index,",
    "pf_has(race->pflags, ability->index)",
    "of_has(clazz->flags, ability->index)",
    "race->el_info[ability->index].res_level !=",
)
assert BIRTH.count("angdroid_fa_birth_ability_name(") == 3
assert "angdroid_game_ui_text(ability->name," not in BIRTH

# Portrait-mode instructions retain the original * @ Esc = ? Ctrl-X keys
# but stop before the interactive question/list region.
instructions = BIRTH.split("static void print_menu_instructions(void)", 1)[1].split(
    "static void finish_with_random_choices(", 1
)[0]
require(instructions,
    "if (fa_birth_compact_layout()) {",
    'fa_birth_prompt_left(zh ? "↑↓ 選擇　Enter 確認"',
    'fa_birth_prompt_left(zh ? "* 隨機　@ 完成"',
    'fa_birth_prompt_left(zh ? "Esc 返回　= 選項"',
    'fa_birth_prompt_left(zh ? "? 說明　Ctrl-X 離開"',
    "HEADER_ROW + 3",
    "return;",
    "BIRTH_MENU_HELPTEXT_ZH : BIRTH_MENU_HELPTEXT",
)
question = BIRTH.split("static enum birth_stage menu_question(", 1)[1].split(
    "static enum birth_stage roller_command(", 1
)[0]
require(question,
    "if (Term && menu_data->hint && Term->wid > QUESTION_COL",
    "angdroid_fa_history_wrap(menu_data->hint, width,",
    "angdroid_fa_history_copy_line(original, sizeof(original),",
    "line < (size_t)(TABLE_ROW - QUESTION_ROW)",
    "Term_putstr(QUESTION_COL, QUESTION_ROW + (int)line, -1,",
    'current_menu->cmd_keys = fa_birth_compact_layout() &&',
)

assert "test_faangband_birth_ability_labels_source.py" in RUNNER
print("PASS: FA typed trait localization and compact birth hints")
