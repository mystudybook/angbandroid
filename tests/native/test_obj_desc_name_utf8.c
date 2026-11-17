/* Exercise the production name grammar helper without JNI/Android. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "../../app/src/main/cpp/angband/obj-desc-name-utf8.h"

static unsigned long checks = 0;
#define CHECK(e) do { assert(e); ++checks; } while (0)

static void expected(const char *fmt, const char *mod, bool plural,
    const char *want)
{
    char out[256];
    size_t n = angdroid_obj_desc_name_format_safe(out, sizeof(out), 0,
        fmt, mod, plural);
    CHECK(n == strlen(want));
    CHECK(strcmp(out, want) == 0);
}

static void examples(void)
{
    char out[4] = "abc";
    const char bad[] = { (char)0xE4, 'X', 0 };
    expected("& Dagger~", NULL, false, "Dagger");
    expected("& Dagger~", NULL, true, "Daggers");
    expected("& Torch~", NULL, true, "Torches");
    expected("& Box~", NULL, true, "Boxes");
    expected("kni|fe|ves|", NULL, false, "knife");
    expected("kni|fe|ves|", NULL, true, "knives");
    expected("Sta|ff|ves|", NULL, true, "Staves");
    expected("& # Potion~", "紅寶石", false, "紅寶石 Potion");
    expected("& # Potion~", "紅寶石", true, "紅寶石 Potions");
    expected("& # Scroll~", NULL, false, " Scroll");
    expected("中文字串", NULL, false, "中文字串");
    expected("魔法|長劍|短劍|", NULL, false, "魔法長劍");
    expected("魔法|長劍|短劍|", NULL, true, "魔法短劍");
    expected("~", NULL, true, "s");
    expected("[|broken", NULL, false, "[");
    expected(bad, NULL, false, "?X");
    expected("紅寶石", NULL, true, "紅寶石");
    CHECK(angdroid_obj_desc_name_format_safe(out, 0, 0, "x", NULL, false) == 0);
    CHECK(strcmp(out, "abc") == 0);
    CHECK(angdroid_obj_desc_name_format_safe(NULL, 4, 0, "x", NULL, false) == 0);
    CHECK(angdroid_obj_desc_name_format_safe(out, 4, 9, "x", NULL, false) == 3);
    CHECK(strcmp(out, "abc") == 0);
    CHECK(angdroid_obj_desc_name_format_safe(out, 4, 0, NULL, NULL, false) == 0);
    CHECK(!out[0]);
}

static void capacities(void)
{
    const char *texts[] = {
        "& Dagger~", "& Scroll~ titled #", "魔法卷軸", "#", "中文|短劍|長劍|",
        "a|b|c|", "1234", "紅寶石", "a # Z", "", "~", "||||"
    };
    size_t i, cap;
    for (i = 0; i < sizeof(texts)/sizeof(texts[0]); ++i) {
        for (cap = 0; cap <= 31; ++cap) {
            char guard[34];
            char *out = guard + 1;
            size_t result, n, pos;
            memset(guard, 0x5A, sizeof(guard));
            result = angdroid_obj_desc_name_format_safe(out, cap, 0,
                texts[i], "中文", (cap % 2) != 0);
            CHECK(guard[0] == 'Z');
            CHECK(guard[33] == 'Z');
            CHECK(guard[cap + 1] == 'Z');
            if (!cap) { CHECK(guard[1] == 'Z'); continue; }
            CHECK(result < cap);
            CHECK(out[result] == 0);
            CHECK(strlen(out) == result);
            for (pos = 0; pos < result; ) {
                uint32_t codepoint;
                n = angdroid_utf8_next_scalar(out + pos, &codepoint);
                CHECK(n != 0 && codepoint != 0xFFFDu);
                pos += n;
            }
            CHECK(pos == result);
        }
    }
}

int main(void)
{
    examples();
    capacities();
    printf("PASS: object descriptor name UTF-8 grammar (%lu assertions)\n", checks);
    return 0;
}
