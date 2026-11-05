/* Bounded scroll title formatting without changing the canonical name. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-scroll-title.h"

int main(void)
{
    char out[96], other[96], tiny[5] = "safe";
    char one[1] = { 'X' };
    const char *scroll = "\"lorem ipsum\"";
    assert(angdroid_zh_tw_scroll_title(out, sizeof(out), scroll, false));
    assert(strcmp(out, "「lorem ipsum」") == 0);
    assert(angdroid_zh_tw_scroll_title(other, sizeof(other), scroll, true));
    assert(strcmp(other, "題為「lorem ipsum」的卷軸") == 0);
    assert(strcmp(scroll, "\"lorem ipsum\"") == 0);
    assert(angdroid_zh_tw_scroll_title(out, sizeof(out), "\"lorem ipsun\"", true));
    assert(strcmp(out, other) != 0);
    assert(!angdroid_zh_tw_scroll_title(tiny, sizeof(tiny), scroll, true));
    assert(tiny[0] == '\0');
    assert(!angdroid_zh_tw_scroll_title(one, sizeof(one), scroll, false));
    assert(one[0] == '\0');
    assert(!angdroid_zh_tw_scroll_title(out, sizeof(out), NULL, false));
    assert(!angdroid_zh_tw_scroll_title(out, sizeof(out), "unquoted", true));
    assert(!angdroid_zh_tw_scroll_title(out, sizeof(out), "\"a\"", false));
    assert(!angdroid_zh_tw_scroll_title(out, sizeof(out),
                                       "\"a very very long scroll title\"", false));
    assert(!angdroid_zh_tw_scroll_title(out, sizeof(out), "\"a 123\"", false));
    assert(!angdroid_zh_tw_scroll_title(out, sizeof(out), "\"foo\"bar\"", false));
    assert(!angdroid_zh_tw_scroll_title(out, sizeof(out), "\"foo \"", false));
    assert(!angdroid_zh_tw_scroll_title(NULL, 96, scroll, false));
    puts("PASS: C99 scroll title wrappers preserve opaque identifiers");
    return 0;
}
