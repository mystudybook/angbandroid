/* All-or-nothing 14/10-cell price and weight text in bilingual stores. */
#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/store-detail-text.h"

int main(void)
{
    char out[80], tiny[4] = "abc";
    assert(angdroid_store_price_text(out, sizeof(out), 123, false, false));
    assert(strcmp(out, "      123    ") == 0);
    assert(angdroid_store_price_text(out, sizeof(out), 123, true, false));
    assert(strcmp(out, "      123 avg") == 0);
    assert(angdroid_store_price_text(out, sizeof(out), 123, true, true));
    assert(strcmp(out, "      123 均價") == 0);
    assert(angdroid_store_price_text(out, sizeof(out), 999999999, true, true));
    assert(strcmp(out, "999999999 均價") == 0);
    /* Large price preserves its numeric value; suppress the longer suffix. */
    assert(angdroid_store_price_text(out, sizeof(out), 1000000000, true, true));
    assert(strcmp(out, "1000000000") == 0);
    assert(angdroid_store_price_text(out, sizeof(out), 1000000000, true, false));
    assert(strcmp(out, "1000000000 avg") == 0);
    assert(!angdroid_store_price_text(out, sizeof(out), LONG_MAX, true, true));
    assert(out[0] == '\0');
    assert(!angdroid_store_price_text(tiny, sizeof(tiny), 12345, false, true));
    assert(tiny[0] == '\0');
    assert(!angdroid_store_price_text(NULL, 80, 123, false, false));

    assert(angdroid_store_weight_text(out, sizeof(out), 165, true));
    assert(strcmp(out, " 16.5 磅") == 0);
    assert(angdroid_store_weight_text(out, sizeof(out), 165, false));
    assert(strcmp(out, " 16.5 lb") == 0);
    assert(angdroid_store_weight_text(out, sizeof(out), 1234567, true));
    assert(strcmp(out, "123456.7") == 0);
    assert(!angdroid_store_weight_text(out, sizeof(out), INT_MAX, true));
    assert(out[0] == '\0');
    assert(!angdroid_store_weight_text(tiny, sizeof(tiny), 165, true));
    assert(tiny[0] == '\0');
    assert(!angdroid_store_weight_text(NULL, 80, 165, false));
    puts("PASS: store prices and weight values are complete and bilingual");
    return 0;
}
