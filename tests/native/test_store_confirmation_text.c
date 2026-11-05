#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/store-confirmation-text.h"

static void check(enum angdroid_store_quantity_action action,
                  int max, int held, const char *expected)
{
    char out[160];
    assert(angdroid_store_quantity_prompt(out, sizeof(out),
                                          action, max, held));
    assert(strcmp(out, expected) == 0);
}

int main(void)
{
    char out[160], small[8];
    int k;
    assert(angdroid_store_explicit_yes(true, 'y'));
    assert(angdroid_store_explicit_yes(true, 'Y'));
    assert(!angdroid_store_explicit_yes(true, 'n'));
    assert(!angdroid_store_explicit_yes(true, 'N'));
    assert(!angdroid_store_explicit_yes(true, '\n'));
    assert(!angdroid_store_explicit_yes(true, 0));
    assert(!angdroid_store_explicit_yes(true, 0xE000));
    assert(!angdroid_store_explicit_yes(false, 'Y'));
    assert(!angdroid_store_explicit_yes(false, '\n'));
    for (k = 0; k < 256; ++k)
        assert(angdroid_store_explicit_yes(true, (unsigned int)k) ==
               (k == 'y' || k == 'Y'));
    check(ANGDROID_STORE_Q_BUY, 15, 3,
          "購買數量（最多 15，已持有 3，* 全部）：");
    check(ANGDROID_STORE_Q_TAKE, 4, 1,
          "取出數量（最多 4，已持有 1，* 全部）：");
    check(ANGDROID_STORE_Q_BUY, 7, 0,
          "購買數量（最多 7，* 全部）：");
    check(ANGDROID_STORE_Q_SELL, 10, 0,
          "出售數量（最多 10，* 全部）：");
    check(ANGDROID_STORE_Q_GIVE, 1, 0,
          "交出數量（最多 1，* 全部）：");
    check(ANGDROID_STORE_Q_STASH, 3, 0,
          "存放數量（最多 3，* 全部）：");
    check(ANGDROID_STORE_Q_BUY, INT_MAX, INT_MAX,
          "購買數量（最多 2147483647，已持有 2147483647，* 全部）：");
    assert(!angdroid_store_quantity_prompt(out, sizeof(out),
         ANGDROID_STORE_Q_BUY, -1, 0));
    assert(out[0] == '\0');
    assert(!angdroid_store_quantity_prompt(out, sizeof(out),
         (enum angdroid_store_quantity_action)99, 1, 0));
    assert(out[0] == '\0');
    assert(!angdroid_store_quantity_prompt(small, sizeof(small),
         ANGDROID_STORE_Q_SELL, 10, 0));
    assert(small[0] == '\0');
    assert(!angdroid_store_quantity_prompt(NULL, sizeof(out),
         ANGDROID_STORE_Q_SELL, 10, 0));
    puts("PASS: explicit Y confirmation and bounded bilingual store quantity prompts");
    return 0;
}
