#!/usr/bin/env python3
"""Source-contract checks for bilingual status, speed and command prompts.

These checks are static: native compilation and gameplay require the local
C99/NDK toolchain, and are not implied by passing this test.
"""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp"
ang_display = (CPP / "angband/ui-display.c").read_text(encoding="utf-8")
fa_display = (CPP / "faangband/ui-display.c").read_text(encoding="utf-8")
ang_input = (CPP / "angband/ui-input.c").read_text(encoding="utf-8")
fa_input = (CPP / "faangband/ui-input.c").read_text(encoding="utf-8")
shared = (CPP / "angband/zh-tw-ui.h").read_text(encoding="utf-8")
run = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")


def function(source: str, signature: str) -> str:
    start = source.index(signature)
    return source[start:source.index("\n}\n", start) + 2]


for variant, source in (("Angband", ang_display), ("FAangband", fa_display)):
    speed = function(source, "static int prt_speed_aux(")
    assert 'angdroid_zh_tw_enabled() ? "加速" : "Fast"' in speed, variant
    assert 'angdroid_zh_tw_enabled() ? "減速" : "Slow"' in speed, variant
    assert '(int)angdroid_utf8_display_columns(buf)' in speed, variant
    assert 'OPT(player, effective_speed)' in speed, variant
    assert '"%s (%+d)"' in speed and '"%s (%d.%dx)"' in speed, variant

    sidebar = function(source, "static void prt_speed(")
    assert 'if (angdroid_zh_tw_enabled())' in sidebar, variant
    assert 'c_put_str(attr, "           ", row, col);' in sidebar, variant
    assert 'format("%-11s", buf)' in sidebar, variant
    compact = function(source, "static int prt_speed_short(")
    assert "return len+1;" in compact, variant

    state = function(source, "static size_t prt_state(")
    chinese, english = state.split("/* Displayed states are resting and repeating */", 1)
    assert 'if (angdroid_zh_tw_enabled()) {' in chinese, variant
    assert 'char zh_text[40] = ""' in chinese, variant
    for special in ('"休息 %d00"', '"休息 %d"',
                    '"休息 *****"', '"休息 &&&&&"', '"休息 !!!!!"',
                    '"重複 %d00"', '"重複 %d"'):
        assert special in chinese, (variant, special)
    assert 'n / 100' in chinese and 'nrepeats / 100' in chinese, variant
    assert "return zh_text[0] ?" in chinese, variant
    assert 'text[9]' not in chinese, variant
    assert 'my_strcpy(text, "Rest      ", sizeof(text));' in english, variant
    assert '"Rep. %3d00"' in english and '"Repeat %3d"' in english, variant
    assert "return strlen(text) + 1;" in english, variant

assert '"休息 $$$$$"' in function(fa_display, "static size_t prt_state(")
assert 'REST_SUNLIGHT' in function(fa_display, "static size_t prt_state(")
assert 'fa_status_put(attr, zh_text, row, col)' in fa_display
assert 'ang_status_put(attr, zh_text, row, col)' in ang_display
assert 'angdroid_menu_copy_columns(visible, sizeof(visible),' in ang_display
assert '"回城" : "Recall"' in ang_display
assert '"深入" : "Descent"' in ang_display
assert '"學習 (%d)" : "Study (%d)"' in ang_display

assert '{"Command: ", "指令："}' in shared
assert '{"Control: ", "控制鍵："}' in shared
assert shared.count('{"Command: ", "指令："}') == 1
assert shared.count('{"Control: ", "控制鍵："}') == 1
assert '{"Repeat: %d", "重複次數：%d"}' in shared
assert 'angdroid_game_ui_text("Repeat: %d",' in fa_input
assert 'angdroid_game_ui_text("Repeat: %d",' in ang_input
assert ang_input.count('get_com_ex(angdroid_game_ui_text(') == 3
assert 'const char *display = angdroid_game_ui_text(' in fa_input
assert fa_input.count('get_com_ex("Command: ", &ke)') == 2
assert 'get_com_ex("Control: ", &ke)' in fa_input
assert "test_speed_rest_repeat_locale_source.py" in run

print("PASS: 2-variant speed/rest/repeat locale contracts and command prompts")
