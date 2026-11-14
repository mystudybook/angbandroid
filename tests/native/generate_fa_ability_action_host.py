#!/usr/bin/env python3
"""Compile the actual FAangband gain_spec_action() in a tiny C99 host harness.

Only menu/event/ability dependencies are mocked.  The function under test
is extracted verbatim from production ui-player-properties.c, so a changed
native callback is compiled on the next local verification run.
"""
from pathlib import Path
import re
import sys

ROOT = Path(__file__).resolve().parents[2]
SOURCE = ROOT / "app/src/main/cpp/faangband/ui-player-properties.c"
pattern = (
    r"(?ms)^static bool gain_spec_action\(struct menu \*menu, "
    r"const ui_event \*e, int oid\)\s*\{.*?^\}"
)
matched = re.search(pattern, SOURCE.read_text(encoding="utf-8"))
if not matched:
    raise SystemExit("Cannot extract production gain_spec_action()")

prefix = r"""
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

enum { EVT_MOUSE = 1, EVT_KBRD = 2, EVT_SELECT = 4 };
typedef struct ui_event {
    int type;
    struct { int code; } key;
} ui_event;
struct menu { void *data; int cursor; };
struct spec_menu_data {
    int *specialties;
    int spec_known;
    int selected_spec;
};
static int page_events;
static bool allow_selection = true;

static void *menu_priv(struct menu *menu)
{
    return menu ? menu->data : NULL;
}

static bool fa_ability_handle_page(struct menu *menu,
    const ui_event *event, int oid)
{
    (void)menu;
    (void)oid;
    if (event && event->type == EVT_KBRD &&
        (event->key.code == '[' || event->key.code == ']')) {
        ++page_events;
        return true;
    }
    return false;
}

static bool check_specialty_gain(int specialty)
{
    return allow_selection && (specialty == 42 || specialty == 73);
}
"""

suffix = r"""
static unsigned long checks;
#define CHECK(x) do { \
    if (!(x)) { \
        fprintf(stderr, "FAIL line %d: %s\n", __LINE__, #x); \
        return 1; \
    } \
    ++checks; \
} while (0)

int main(void)
{
    int ids[] = {42, 73};
    struct spec_menu_data data = {ids, 2, 0};
    struct menu menu = {&data, 0};
    ui_event mouse = {EVT_MOUSE, {0}};
    ui_event select = {EVT_SELECT, {0}};
    ui_event page = {EVT_KBRD, {'['}};
    ui_event other = {EVT_KBRD, {'z'}};
    struct menu empty_menu = {NULL, 0};
    bool context_hook_called;

    /* menu_select() tries the selected row's handler before calling the
     * context hook for an outside-list tap. The raw tap must pass through. */
    context_hook_called = !gain_spec_action(&menu, &mouse, 0);
    CHECK(context_hook_called);
    CHECK(data.selected_spec == 0);
    CHECK(!gain_spec_action(&empty_menu, &mouse, 0));
    CHECK(!gain_spec_action(&menu, NULL, 0));

    /* Browsing the explanation is not a specialty selection. */
    CHECK(gain_spec_action(&menu, &page, 0));
    CHECK(page_events == 1);
    CHECK(data.selected_spec == 0);
    CHECK(gain_spec_action(&menu, &other, 0));
    CHECK(data.selected_spec == 0);

    /* Valid row emits a selection; upper/lower invalid rows are consumed. */
    CHECK(!gain_spec_action(&menu, &select, 0));
    CHECK(data.selected_spec == 42);
    data.selected_spec = 0;
    CHECK(!gain_spec_action(&menu, &select, 1));
    CHECK(data.selected_spec == 73);
    data.selected_spec = 0;
    CHECK(gain_spec_action(&menu, &select, -1));
    CHECK(data.selected_spec == 0);
    CHECK(gain_spec_action(&menu, &select, 2));
    CHECK(data.selected_spec == 0);

    /* A stale record must not reopen a confirmation dialogue. */
    allow_selection = false;
    CHECK(gain_spec_action(&menu, &select, 0));
    CHECK(data.selected_spec == 0);
    CHECK(!gain_spec_action(&menu, &mouse, 0));

    printf("PASS: %lu FA specialty native action checks\n", checks);
    return 0;
}
"""

if len(sys.argv) != 2:
    raise SystemExit("usage: generate_fa_ability_action_host.py OUTPUT.c")
Path(sys.argv[1]).write_text(
    prefix + "\n" + matched.group(0) + "\n" + suffix, encoding="utf-8"
)
