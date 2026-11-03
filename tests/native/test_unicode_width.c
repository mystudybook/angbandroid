#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include "../../app/src/main/cpp/curses/unicode_width.h"

static void check(uint32_t codepoint, int expected)
{
    assert(angdroid_terminal_columns(codepoint) == expected);
}

int main(void)
{
    check('A', 1);
    check('@', 1);
    check('9', 1);
    check(0x00DFu, 1);   /* ß */
    check(0x4E2Du, 2);   /* 中 */
    check(0x9F8Du, 2);   /* 龍 */
    check(0x528Du, 2);   /* 劍 */
    check(0x3002u, 2);   /* 。 */
    check(0x3042u, 2);   /* あ */
    check(0xD55Cu, 2);   /* 한 */
    check(0xFF21u, 2);   /* Ａ */
    check(0x20000u, 2); /* CJK extension B */
    check(0x303Fu, 1);
    check(0xE000u, 1);  /* Game private-use special keys */
    puts("PASS: 14 native terminal width classification cases");
    return 0;
}
