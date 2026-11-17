/* Native C99: narrow FAangband name + enhancement + status composition.
 * Production helpers must never leave unmatched brackets or half UTF-8.
 */
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/faangband/obj-desc-basic-utf8.h"
#include "../../app/src/main/cpp/faangband/obj-desc-details-utf8.h"
#include "../../app/src/main/cpp/angband/obj-desc-effect-utf8.h"

static unsigned long checks;
#define CHECK(expression) do { assert(expression); ++checks; } while (0)

static void check_valid_utf8(const char *s)
{
    size_t pos = 0, len = strlen(s);
    while (pos < len) {
        uint32_t scalar;
        size_t bytes = angdroid_utf8_next_scalar(s + pos, &scalar);
        CHECK(bytes > 0);
        CHECK(scalar != 0xFFFDu);
        CHECK(bytes <= len - pos);
        pos += bytes;
    }
    CHECK(pos == len);
}

/* English fallback and already-Chinese names use the same downstream
 * sequence. A failed base is a blank screen label, not a naked status. */
static void chain_at_every_capacity(const char *base)
{
    const int mods[] = { 3, -2 };
    const char *notes[] = { "玩家註記", "受詛咒" };
    const char *groups[] = {
        " <+3, -2>", " (+4,-1)", " (2 charging)",
        " {玩家註記, 受詛咒}"
    };
    size_t cap, i;
    for (cap = 0; cap <= 128; ++cap) {
        char memory[132], expected[160], *out = memory + 1;
        size_t end = 0;
        bool base_ok;
        memset(memory, '#', sizeof(memory));
        expected[0] = '\0';
        if (cap) out[0] = '\0';
        base_ok = angdroid_obj_desc_append_suffix_atomic(
            out, cap, &end, base, false);
        if (base_ok) {
            bool results[4];
            strcpy(expected, base);
            results[0] = angdroid_faangband_append_modifiers(
                out, cap, &end, mods, 2);
            results[1] = angdroid_faangband_append_detailf(
                out, cap, &end, " (%+d,%+d)", 4, -1);
            results[2] = angdroid_faangband_append_detailf(
                out, cap, &end, " (%d charging)", 2);
            results[3] = angdroid_faangband_append_inscription(
                out, cap, &end, notes, 2);
            for (i = 0; i < 4; ++i) {
                bool fit = strlen(expected) + strlen(groups[i]) < cap;
                CHECK(results[i] == fit);
                if (fit) strcat(expected, groups[i]);
            }
        }
        CHECK(memory[0] == '#');
        CHECK(memory[cap + 1] == '#');
        CHECK(memory[131] == '#');
        if (!cap) {
            CHECK(!base_ok);
            CHECK(out[0] == '#');
        } else {
            CHECK(base_ok == (strlen(base) < cap));
            CHECK(strcmp(out, expected) == 0);
            CHECK(end == strlen(out));
            CHECK(end < cap);
            check_valid_utf8(out);
            /* Valid UTF-8 isn't enough: names and suffixes must be
             * complete units in their original order, never fragments. */
        }
    }
}

static void short_basic_labels(void)
{
    struct label_case {
        bool unknown, prefix, translated;
        const char *expected;
    };
    const struct label_case cases[] = {
        { false, false, true, "（空）" },
        { true, true, true, "未知物品" },
        { false, true, false, "(nothing)" },
        { true, true, false, "an unknown item" },
        { true, false, false, "unknown item" }
    };
    size_t i, cap;
    for (i = 0; i < sizeof(cases)/sizeof(cases[0]); ++i)
        for (cap = 0; cap <= 32; ++cap) {
            char memory[36], *out = memory + 1;
            size_t n;
            memset(memory, 'Z', sizeof(memory));
            n = angdroid_faangband_basic_item_text(out, cap,
                cases[i].unknown, cases[i].prefix,
                cases[i].translated);
            CHECK(memory[0] == 'Z');
            CHECK(memory[cap + 1] == 'Z');
            CHECK(memory[35] == 'Z');
            if (!cap) {
                CHECK(n == 0 && out[0] == 'Z');
            } else if (cap > strlen(cases[i].expected)) {
                CHECK(n == strlen(cases[i].expected));
                CHECK(strcmp(out, cases[i].expected) == 0);
            } else {
                CHECK(n == 0);
                CHECK(out[0] == '\0');
            }
        }
}

static void original_effect_fallback(void)
{
    char out[64];
    size_t end = 0;
    out[0] = '\0';
    CHECK(angdroid_obj_desc_append_suffix_atomic(
        out, sizeof(out), &end, "Long Sword", false));
    CHECK(angdroid_obj_desc_append_flavor_effect(
        out, sizeof(out), &end, "Healing", false));
    CHECK(strcmp(out, "Long Sword of Healing") == 0);
    CHECK(!angdroid_obj_desc_append_flavor_effect(
        out, end + 4, &end, "Healing", false));
    CHECK(strcmp(out, "Long Sword of Healing") == 0);

    strcpy(out, "長劍");
    end = strlen(out);
    CHECK(angdroid_obj_desc_append_suffix_atomic(
        out, sizeof(out), &end, "（諾多精靈）", false));
    CHECK(strcmp(out, "長劍（諾多精靈）") == 0);
    check_valid_utf8(out);
}

int main(void)
{
    chain_at_every_capacity("Long Sword");
    chain_at_every_capacity("長劍");
    short_basic_labels();
    original_effect_fallback();
    printf("PASS: FA narrow object-name/group chaining (%lu checks)\n", checks);
    return 0;
}
