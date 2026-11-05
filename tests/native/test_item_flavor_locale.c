/* Unknown item appearances are display-only and cannot reveal kind names. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-flavor-names.h"
static void check(const char *english, const char *chinese)
{
    const char *actual = angdroid_zh_tw_flavor_name(english);
    assert(actual != NULL);
    assert(strcmp(actual, chinese) == 0);
}
int main(void)
{
    assert(ANGDROID_ZH_TW_FLAVOR_NAMES == 202);
    check("Alexandrite", "亞歷山大變石");
    check("Ruby", "紅寶石");
    check("Oak", "橡木");
    check("Copper-Plated", "鍍銅");
    check("Light Blue", "淺藍色");
    check("Viscous Pink", "黏稠粉紅色");
    check("Plain Gold", "純金");
    assert(angdroid_zh_tw_flavor_name("unknown flavor") == NULL);
    assert(angdroid_zh_tw_flavor_name("ruby") == NULL);
    assert(angdroid_zh_tw_flavor_name("") == NULL);
    assert(angdroid_zh_tw_flavor_name(NULL) == NULL);
    puts("PASS: all 202 canonical appearance labels with English fallback");
    return 0;
}
