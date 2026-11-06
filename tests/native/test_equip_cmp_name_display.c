/* Bounded CJK equipment comparison ROW text, not canonical sort/export.
 * Tests complete UTF-8 scalar boundaries, suffix names and stale-cell clearing.
 */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/equip-compare-name-display.h"

static int checked;
static void verify(const char *input, size_t cells, bool tail,
                   const char *expected)
{
    char buffer[256];
    assert(angdroid_equip_cmp_format_short_name(
        buffer, sizeof(buffer), input, cells, tail));
    assert(strcmp(buffer, expected) == 0);
    assert(angdroid_utf8_display_columns(buffer) == cells);
    checked += 3;
}
int main(void)
{
    char tiny[5] = "old";
    char out[64] = "old";

    verify("火焰之劍", 8, false, "火焰之劍");
    verify("火焰之劍", 7, false, "火焰之 ");
    verify("火焰之劍", 6, false, "火焰之");
    verify("火焰之劍", 1, false, " ");
    verify("前綴天地玄黃", 6, true, "地玄黃");
    verify("前綴天地玄黃", 5, true, "玄黃 ");
    verify("前綴天地玄黃", 8, true, "天地玄黃");
    verify("Ring of Power", 7, false, "Ring of");
    verify("Ring of Power", 7, true, "f Power");
    verify("短", 6, false, "短    ");
    verify("", 3, false, "   ");
    verify("a中文戒指", 5, false, "a中文");
    verify("a中文戒指", 4, false, "a中 ");
    verify("A", 1, false, "A");
    verify("\xE4\xB8", 3, false, "?? ");

    assert(!angdroid_equip_cmp_format_short_name(
        tiny, sizeof(tiny), "短", 7, false));
    assert(tiny[0] == '\0');
    checked += 2;
    assert(!angdroid_equip_cmp_format_short_name(out, sizeof(out), NULL, 6, false));
    assert(out[0] == '\0');
    checked += 2;
    assert(!angdroid_equip_cmp_format_short_name(out, sizeof(out), "短", 0, false));
    assert(out[0] == '\0');
    checked += 2;
    assert(!angdroid_equip_cmp_format_short_name(NULL, sizeof(out), "短", 3, false));
    checked += 1;
    assert(!angdroid_equip_cmp_format_short_name(out, 0, "短", 3, false));
    checked += 1;
    printf("PASS: equipment comparison CJK display names (%d assertions)\n", checked);
    return 0;
}
