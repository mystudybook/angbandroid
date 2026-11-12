#!/usr/bin/env python3
"""FAangband race/class trait paging is presentation-only and touch accessible.

This source-level guard is intentionally separate from device/NDK testing.
"""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
BIRTH = (ROOT / "app/src/main/cpp/faangband/ui-birth.c").read_text(encoding="utf-8")
RUNNER = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")


def part(start, end):
    assert start in BIRTH and end in BIRTH, (start, end)
    return BIRTH.split(start, 1)[1].split(end, 1)[0]


def need(body, *terms):
    for term in terms:
        assert term in body, term


state = part("static int fa_birth_detail_page = 0;",
             "static bool fa_birth_compact_layout(void)")
need(state,
    "static int fa_birth_detail_page_count = 1;",
    "static int fa_birth_detail_last_oid = -1;",
    "static bool fa_birth_detail_last_class = false;",
    "static void fa_birth_detail_for_choice(int oid, bool class_choice)",
    "oid != fa_birth_detail_last_oid",
    "fa_birth_detail_page = 0;",
    "static bool fa_birth_detail_change_page(int direction)",
    "if (next < 0 || next >= fa_birth_detail_page_count)",
    "fa_birth_detail_page = next;",
)
assert "cmdq_push(" not in state

layout = part("static void fa_birth_configure_layout(void)",
              "typedef void (*browse_f)")
need(layout, "Term->wid != fa_birth_layout_width",
    "Term->hgt != fa_birth_layout_height",
    "fa_birth_detail_page = 0;",
    "fa_birth_detail_page_count = 1;")

rows = part("static void fa_birth_help_line(", "static void race_help(")
need(rows,
    "per_page = Term ? MAX(0, Term->hgt - 2 - first) : 0;",
    "offset = *row - first - fa_birth_detail_page * per_page;",
    "offset >= 0 && offset < per_page",
    "screen_row >= 0 && screen_row < Term->hgt - 2",
    "angdroid_menu_copy_columns(visible, sizeof(visible),",
    "fa_birth_detail_page_count = MAX(1,",
    "fa_birth_detail_page >= fa_birth_detail_page_count",
    'format("特性 %d/%d：按 [ 上頁、] 下頁"',
    'format("Traits %d/%d: [ prev, ] next"',
)
assert "cmdq_push(" not in rows
assert "CMD_CHOOSE_CLASS" not in rows
assert "CMD_CHOOSE_RACE" not in rows

race = part("static void race_help(", "static void class_help(")
clazz = part("static void class_help(", "static bool use_context_menu_birth(")
need(race, "fa_birth_detail_for_choice(i, false);",
    "fa_birth_compact_details(r, NULL);")
need(clazz, "fa_birth_detail_for_choice(i, true);",
    "fa_birth_compact_details(r, c);")

touch = part("static bool use_context_menu_birth(",
             "static void init_birth_menu(")
need(touch,
    "ACT_CTX_BIRTH_PREV_DETAILS",
    "ACT_CTX_BIRTH_NEXT_DETAILS",
    "fa_birth_detail_page_count > 1",
    "in->mouse.y == Term->hgt - 2",
    "fa_birth_detail_change_page(in->mouse.x < middle ? -1 : 1);",
    "out->type = EVT_SWITCH;",
    'menu_dynamic_add_label(m, angdroid_zh_tw_enabled() ?',
    '"特性上一頁"', '"特性下一頁"',
    "fa_birth_detail_change_page(-1);",
    "fa_birth_detail_change_page(1);",
)
# The touch Help button may open instructions, but must never send the
# player back to BIRTH_RESET and discard creation progress.
help_case = touch.split("case ACT_CTX_BIRTH_HELP:", 1)[1].split(
    "case ACT_CTX_BIRTH_PREV_DETAILS:", 1)[0]
need(help_case, "do_cmd_help();", "out->type = EVT_SWITCH;", "break;")
assert "menu_data->stage_inout = BIRTH_RESET;" not in help_case

question = part("static enum birth_stage menu_question(",
                "static enum birth_stage roller_command(")
need(question,
    'current_menu->cmd_keys = fa_birth_compact_layout() &&',
    'current != BIRTH_ROLLER_CHOICE ?',
    '"?=*@[]\\x18"',
    "cx.key.code == '[' || cx.key.code == ']'",
    "fa_birth_detail_change_page(",
    "menu_refresh(current_menu, false);",
    "next = BIRTH_RESET;",
    "continue;",
    "cmdq_push(choice_command);",
    'cmd_set_arg_choice(cmdq_peek(), "choice", current_menu->cursor);',
)
pager_start = question.index("cx.key.code == '[' || cx.key.code == ']'")
pager_end = question.index("/* '*' chooses an option", pager_start)
assert "cmdq_push(" not in question[pager_start:pager_end]

enter = part("static void ui_enter_birthscreen(",
             "static void ui_leave_birthscreen(")
need(enter,
    "fa_birth_detail_page = 0;",
    "fa_birth_detail_page_count = 1;",
    "fa_birth_detail_last_oid = -1;",
    "fa_birth_layout_width = -1;",
)

assert "test_faangband_birth_traits_paging_source.py" in RUNNER
print("PASS: FA birth traits paging preserves choice ID and supports touch")
