/* Menu shortcut strings must not match zero or truncated special keys. */
#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include "../../app/src/main/cpp/angband/menu-shortcut-code.h"

int main(void)
{
    unsigned int code;
    unsigned int checked = 0;
    for (code = 0; code <= 0x10000U; ++code) {
        int expect = (code != 0 && code <= 0xff);
        assert(!!angdroid_menu_shortcut_is_byte(code) == expect);
        ++checked;
    }
    /* Both of these could alias ASCII '/' if truncated to char. */
    assert(!angdroid_menu_shortcut_is_byte(0x1002fU));
    assert(!angdroid_menu_shortcut_is_byte(0x2002fU));
    assert(!angdroid_menu_shortcut_is_byte(UINT_MAX));
    assert(!angdroid_menu_shortcut_is_byte((unsigned int)-1));
    assert(angdroid_menu_shortcut_is_byte('/'));
    assert(angdroid_menu_shortcut_is_byte('p'));
    printf("PASS: %u native byte-only menu shortcut code checks\n", checked + 6);
    return 0;
}
