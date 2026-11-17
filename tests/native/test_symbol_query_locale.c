/* Original Angband symbol lookup: all 36 tvals, display-only labels and
 * English fallback. Standalone C99; no Android SDK or engine required.
 */
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "../../app/src/main/cpp/curses/unicode_display.h"
#include "../../app/src/main/cpp/angband/zh-tw-symbol-query.h"

int main(void)
{
    char buf[128];
    char tiny[5];
    int count = 0;
#define TV(id, label) do { \
    const char *name = angdroid_symbol_tval_name(label, 1); \
    assert(name && strcmp(name, label) != 0); \
    assert(strcmp(angdroid_symbol_tval_name(label, 0), label) == 0); \
    ++count; \
} while (0);
#include "../../app/src/main/cpp/angband/list-tvals.h"
#undef TV
    assert(count == ANGDROID_ZH_TW_SYMBOL_TVAL_COUNT);
    assert(strcmp(angdroid_symbol_tval_name("ring", 1), "戒指") == 0);
    assert(strcmp(angdroid_symbol_tval_name("dragon armor", 1), "龍鱗甲") == 0);
    assert(strcmp(angdroid_symbol_tval_name("missing-category", 1),
                  "missing-category") == 0);
    assert(angdroid_symbol_tval_name(NULL, 1) == NULL);
    angdroid_symbol_query_format(buf, sizeof(buf), '!', "potion",
                                 angdroid_symbol_tval_name("potion", 1), 1);
    assert(strcmp(buf, "! - 藥水。") == 0);
    angdroid_symbol_query_format(buf, sizeof(buf), '#', "granite wall",
                                 "花崗岩牆", 1);
    assert(strcmp(buf, "# - 花崗岩牆。") == 0);
    angdroid_symbol_query_format(buf, sizeof(buf), '#', "granite wall",
                                 "花崗岩牆", 0);
    assert(strcmp(buf, "# - granite wall.") == 0);
    angdroid_symbol_query_format(buf, sizeof(buf), '?', "unknown",
                                 "unknown", 1);
    assert(strcmp(buf, "? - unknown.") == 0);
    angdroid_symbol_query_format(tiny, sizeof(tiny), '!', "potion", "藥水", 1);
    assert(tiny[sizeof(tiny)-1] == '\0');
    angdroid_symbol_query_format(NULL, 0, '!', "potion", "藥水", 1);
    /* Byte-truncated localized labels must never emit partial UTF-8.
     * Test every byte capacity around two Han glyphs and punctuation. */
    {
        char guarded[32];
        size_t cap;
        for (cap = 1; cap <= 16; ++cap) {
            size_t i = 0;
            memset(guarded, 'Q', sizeof guarded);
            angdroid_symbol_query_format(guarded, cap,
                '!', "potion", "藥水", 1);
            assert(guarded[cap] == 'Q');
            assert(strlen(guarded) < cap);
            while (guarded[i]) {
                uint32_t cp = 0;
                size_t length = angdroid_utf8_next_scalar(guarded + i, &cp);
                assert(length != 0);
                assert(!(length == 1 && cp == 0xFFFDu &&
                         (unsigned char)guarded[i] >= 0x80u));
                i += length;
            }
            assert(i == strlen(guarded));
            if (cap == 6 || cap == 7)
                assert(strcmp(guarded, "! - ") == 0);
            if (cap == 9 || cap == 10)
                assert(strcmp(guarded, "! - 藥") == 0);
            if (cap == 12 || cap == 13)
                assert(strcmp(guarded, "! - 藥水") == 0);
            if (cap == 14)
                assert(strcmp(guarded, "! - 藥水。") == 0);
        }
        angdroid_symbol_query_format(guarded, 8,
            '!', "cat", "🐱", 1);
        assert(strcmp(guarded, "! - ") == 0);
        angdroid_symbol_query_format(guarded, 12,
            '!', "cat", "🐱", 1);
        assert(strcmp(guarded, "! - 🐱。") == 0);
    }
    puts("PASS: symbol query labels and source-key fallbacks");
    return 0;
}
