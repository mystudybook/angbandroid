/* FAangband: exercise the production shared name formatter in isolation.
 * This targets the historical literal buf[end++] overflow in obj-desc.c.
 */
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/obj-desc-name-utf8.h"

static unsigned long checks;
#define CHECK(x) do { assert(x); ++checks; } while (0)

static void expect(const char *fmt, const char *modifier, bool plural,
    const char *expected)
{
    char out[256];
    size_t end = angdroid_obj_desc_name_format_safe(
        out, sizeof(out), 0, fmt, modifier, plural);
    CHECK(end == strlen(expected));
    CHECK(strcmp(out, expected) == 0);
}

static void original_grammar(void)
{
    char out[32];
    size_t end;
    expect("& Sword~", NULL, false, "Sword");
    expect("& Sword~", NULL, true, "Swords");
    expect("& Box~", NULL, true, "Boxes");
    expect("& Torch~", NULL, true, "Torches");
    expect("& Sta|ff|ves|", NULL, false, "Staff");
    expect("& Sta|ff|ves|", NULL, true, "Staves");
    expect("& # Ring~", "Adamantium", false, "Adamantium Ring");
    expect("& # Ring~", "精金", true, "精金 Rings");
    expect("a # of #", "紅寶石", false, "a 紅寶石 of 紅寶石");
    expect("魔法|長劍|短劍|", NULL, false, "魔法長劍");
    expect("魔法|長劍|短劍|", NULL, true, "魔法短劍");
    expect("~", NULL, true, "s");
    expect("a|broken", NULL, false, "a");
    expect("#", NULL, true, "");
    expect("", NULL, false, "");

    strcpy(out, "ABC");
    end = angdroid_obj_desc_name_format_safe(
        out, sizeof(out), 3, "紅寶石", NULL, false);
    CHECK(end == strlen("ABC紅寶石"));
    CHECK(strcmp(out, "ABC紅寶石") == 0);
    CHECK(angdroid_obj_desc_name_format_safe(
        out, 0, 0, "Sword", NULL, false) == 0);
    CHECK(strcmp(out, "ABC紅寶石") == 0);
    CHECK(angdroid_obj_desc_name_format_safe(
        NULL, sizeof(out), 0, "Sword", NULL, false) == 0);
    CHECK(angdroid_obj_desc_name_format_safe(
        out, 4, 80, "Sword", NULL, false) == 3);
    CHECK(strcmp(out, "ABC") == 0);
}

static void boundary_and_utf8(void)
{
    static const struct {
        const char *fmt, *modifier;
    } cases[] = {
        { "& Long Sword~", NULL },
        { "& # Sta|ff|ves|", "大馬士革鋼" },
        { "& Scroll~ titled #", "\"foo bar\"" },
        { "& Wand~ of #", "治療" },
        { "魔法|長劍|短劍|", NULL },
        { "天藍色藥水（治療）", NULL },
        { "a|b|c|", NULL },
        { "1234567890", NULL },
        { "普通物品", NULL },
        { "#", NULL },
        { "~", NULL },
        { "||||", NULL },
        { "a|broken", NULL },
        { "", NULL }
    };
    size_t i, cap, offset;
    for (i = 0; i < sizeof(cases)/sizeof(cases[0]); ++i) {
        for (cap = 0; cap <= 64; ++cap) {
            char guard[68];
            char *out = guard + 1;
            size_t end, pos;
            memset(guard, 'Z', sizeof(guard));
            end = angdroid_obj_desc_name_format_safe(
                out, cap, 0, cases[i].fmt, cases[i].modifier,
                (cap & 1u) != 0);
            CHECK(guard[0] == 'Z');
            CHECK(guard[cap+1] == 'Z');
            CHECK(guard[67] == 'Z');
            if (!cap) {
                CHECK(out[0] == 'Z');
                CHECK(end == 0);
                continue;
            }
            CHECK(end < cap);
            CHECK(out[end] == '\0');
            CHECK(strlen(out) == end);
            pos = 0;
            while (pos < end) {
                uint32_t scalar;
                size_t width = angdroid_utf8_next_scalar(
                    out + pos, &scalar);
                CHECK(width >= 1);
                CHECK(scalar != 0xFFFDu);
                CHECK(pos + width <= end);
                pos += width;
            }
            CHECK(pos == end);
        }
    }

    /* Explicitly reject half of a Chinese character at byte limits. */
    for (offset = 1; offset <= 7; ++offset) {
        char storage[10];
        size_t end;
        memset(storage, 'Q', sizeof(storage));
        end = angdroid_obj_desc_name_format_safe(
            storage, offset, 0, "中文", NULL, false);
        CHECK(end < offset);
        CHECK(storage[end] == '\0');
        CHECK(storage[offset] == 'Q');
        if (offset <= 3) CHECK(end == 0);
        else if (offset <= 6) CHECK(end == 3);
        else CHECK(end == 6);
    }
}

int main(void)
{
    original_grammar();
    boundary_and_utf8();
    printf("PASS: FAangband name grammar, 0..64 byte buffers, UTF-8 (%lu checks)\n",
        checks);
    return 0;
}
