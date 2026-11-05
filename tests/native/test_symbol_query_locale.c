/* Original Angband symbol lookup: all 36 tvals, display-only labels and
 * English fallback. Standalone C99; no Android SDK or engine required.
 */
#include <assert.h>
#include <stdio.h>
#include <string.h>
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
    puts("PASS: symbol query labels and source-key fallbacks");
    return 0;
}
