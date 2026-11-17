/*
 * FAangband: refuse a partially-expanded Chinese monster spell message.
 * Use the actual runtime preflight helper; host C99 without engine linkage.
 */
#include <stdio.h>
#include <stdbool.h>
#include "app/src/main/cpp/faangband/mon-spell-message-locale.h"

static unsigned checks;
#define CHECK(x) do { if (!(x)) { \
    fprintf(stderr, "FAIL line %d: %s\n", __LINE__, #x); \
    return 1; } ++checks; } while (0)

int main(void)
{
    /* UTF-8 byte counts, not the number of Han glyphs. */
    CHECK(angdroid_fa_spell_template_fits("", 1, 320));
    CHECK(!angdroid_fa_spell_template_fits("", 0, 320));
    CHECK(!angdroid_fa_spell_template_fits(NULL, 1024, 320));
    CHECK(!angdroid_fa_spell_template_fits("訊息", 1024, 0));

    CHECK(angdroid_fa_spell_template_fits("{name}{target}", 641, 320));
    CHECK(!angdroid_fa_spell_template_fits("{name}{target}", 640, 320));
    CHECK(angdroid_fa_spell_template_fits(
        "{name}{target}{pronoun}", 1024, 320));
    CHECK(!angdroid_fa_spell_template_fits(
        "{name}{target}{pronoun}{type}", 1024, 320));

    CHECK(angdroid_fa_spell_template_fits(
        "你{target}。", 327, 320));
    CHECK(!angdroid_fa_spell_template_fits(
        "你{target}。", 326, 320));

    /* Largest exact current Chinese template: 679 worst-case bytes. */
    CHECK(angdroid_fa_spell_template_fits(
        "{name}指著{target}尖叫著唸出危險的咒語！", 680, 320));
    CHECK(!angdroid_fa_spell_template_fits(
        "{name}指著{target}尖叫著唸出危險的咒語！", 679, 320));
    CHECK(angdroid_fa_spell_template_fits(
        "{name}指著{target}尖叫著唸出危險的咒語！", 1024, 320));

    /* Non-template braces and incomplete tags cannot bypass the limit. */
    CHECK(angdroid_fa_spell_template_fits("{target", 8, 320));
    CHECK(!angdroid_fa_spell_template_fits("{target", 7, 320));
    CHECK(angdroid_fa_spell_template_fits("正常輸出。", 16, 320));
    CHECK(!angdroid_fa_spell_template_fits("正常輸出。", 15, 320));

    puts("PASS: FAangband dynamic spell budget and UTF-8 byte boundaries");
    printf("CHECKS=%u\n", checks);
    return 0;
}
