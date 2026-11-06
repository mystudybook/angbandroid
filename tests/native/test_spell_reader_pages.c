/* Paged UTF-8 spell-reader regression tests: independent of Angband/JNI. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-spell-reader-pages.h"

static int checks = 0;
#define CHECK(expr) do { assert(expr); ++checks; } while (0)

static size_t one(const char *text, size_t start, size_t width,
    const char *expected)
{
    char line[1024];
    size_t next = 0;
    CHECK(angdroid_spell_reader_next_line(text, start, width, line,
        sizeof(line), &next));
    CHECK(strcmp(line, expected) == 0);
    CHECK(next > start);
    CHECK(angdroid_utf8_display_columns(line) <= width);
    return next;
}

int main(void)
{
    size_t p;
    char line[32];
    size_t next = 0;
    const char *mixed = "A中文B";
    const char *words = "Mana Storm hits.";

    p = one("中文法術", 0, 5, "中文");
    p = one("中文法術", p, 5, "法術");
    CHECK(p == strlen("中文法術"));
    CHECK(angdroid_spell_reader_line_count("中文法術", 5) == 2);

    p = one(mixed, 0, 5, "A中文");
    p = one(mixed, p, 5, "B");
    CHECK(p == strlen(mixed));
    CHECK(angdroid_spell_reader_line_count(mixed, 5) == 2);

    p = one(words, 0, 11, "Mana Storm");
    p = one(words, p, 11, "hits.");
    CHECK(p == strlen(words));
    CHECK(angdroid_spell_reader_line_count(words, 11) == 2);

    p = one("Alpha  Beta", 0, 6, "Alpha");
    p = one("Alpha  Beta", p, 6, "Beta");
    CHECK(p == strlen("Alpha  Beta"));

    p = one("甲\n\n乙", 0, 8, "甲");
    p = one("甲\n\n乙", p, 8, "");
    p = one("甲\n\n乙", p, 8, "乙");
    CHECK(p == strlen("甲\n\n乙"));
    CHECK(angdroid_spell_reader_line_count("甲\n\n乙", 8) == 3);

    CHECK(angdroid_spell_reader_line_count(NULL, 20) == 0);
    CHECK(angdroid_spell_reader_line_count("", 20) == 0);
    CHECK(angdroid_spell_reader_line_count("中文", 0) == 0);
    CHECK(!angdroid_spell_reader_next_line(NULL, 0, 8, line,
        sizeof(line), &next));
    CHECK(!angdroid_spell_reader_next_line("中文", 0, 0, line,
        sizeof(line), &next));
    CHECK(!angdroid_spell_reader_next_line("中文", 0, 1, line,
        sizeof(line), &next));
    CHECK(!angdroid_spell_reader_next_line("中文", 0, 10, line,
        2, &next));
    CHECK(!angdroid_spell_reader_next_line("", 0, 8, line,
        sizeof(line), &next));

    CHECK(angdroid_spell_reader_clamp_scroll(0, 20, 6) == 0);
    CHECK(angdroid_spell_reader_clamp_scroll(20, 20, 6) == 14);
    CHECK(angdroid_spell_reader_clamp_scroll(5, 3, 6) == 0);
    CHECK(angdroid_spell_reader_page_down(0, 20, 6) == 6);
    CHECK(angdroid_spell_reader_page_down(12, 20, 6) == 14);
    CHECK(angdroid_spell_reader_page_down(14, 20, 6) == 14);
    CHECK(angdroid_spell_reader_page_down(0, 2, 6) == 0);
    CHECK(angdroid_spell_reader_page_up(8, 6) == 2);
    CHECK(angdroid_spell_reader_page_up(5, 6) == 0);

    /* A long CJK description must be reachable beyond one short viewport. */
    CHECK(angdroid_spell_reader_line_count(
        "犧牲生命值後吸取敵人的生命，並恢復自身生命與營養。", 9) > 5);

    /* Rotation must preserve the source text position, not the old row. */
    CHECK(angdroid_spell_reader_line_for_offset("中文法術", 5, 0) == 0);
    CHECK(angdroid_spell_reader_line_for_offset("中文法術", 5, 6) == 1);
    CHECK(angdroid_spell_reader_line_for_offset("中文法術", 3, 6) == 2);
    CHECK(angdroid_spell_reader_line_for_offset("中文法術", 7, 6) == 0);
    CHECK(angdroid_spell_reader_line_for_offset("中文法術", 5, 999) == 1);
    CHECK(angdroid_spell_reader_line_for_offset("A中文B", 5, 7) == 1);
    CHECK(angdroid_spell_reader_line_for_offset(NULL, 8, 6) == 0);
    CHECK(angdroid_spell_reader_line_for_offset("", 8, 6) == 0);
    CHECK(angdroid_spell_reader_line_for_offset("中文", 1, 3) == 0);

    /* Title/footer leave; body scrolls only inside the terminal bounds. */
    CHECK(angdroid_spell_reader_tap_action(0, 0, 20, 10) ==
        ANGDROID_SPELL_READER_TAP_BACK);
    CHECK(angdroid_spell_reader_tap_action(5, 9, 20, 10) ==
        ANGDROID_SPELL_READER_TAP_BACK);
    CHECK(angdroid_spell_reader_tap_action(5, 1, 20, 10) ==
        ANGDROID_SPELL_READER_TAP_UP);
    CHECK(angdroid_spell_reader_tap_action(5, 4, 20, 10) ==
        ANGDROID_SPELL_READER_TAP_UP);
    CHECK(angdroid_spell_reader_tap_action(5, 5, 20, 10) ==
        ANGDROID_SPELL_READER_TAP_DOWN);
    CHECK(angdroid_spell_reader_tap_action(5, 8, 20, 10) ==
        ANGDROID_SPELL_READER_TAP_DOWN);
    CHECK(angdroid_spell_reader_tap_action(20, 2, 20, 10) ==
        ANGDROID_SPELL_READER_TAP_IGNORE);
    CHECK(angdroid_spell_reader_tap_action(3, 10, 20, 10) ==
        ANGDROID_SPELL_READER_TAP_IGNORE);
    CHECK(angdroid_spell_reader_tap_action(-1, 2, 20, 10) ==
        ANGDROID_SPELL_READER_TAP_IGNORE);
    CHECK(angdroid_spell_reader_tap_action(2, -1, 20, 10) ==
        ANGDROID_SPELL_READER_TAP_IGNORE);
    CHECK(angdroid_spell_reader_tap_action(1, 1, 3, 10) ==
        ANGDROID_SPELL_READER_TAP_IGNORE);
    CHECK(angdroid_spell_reader_tap_action(1, 1, 20, 3) ==
        ANGDROID_SPELL_READER_TAP_IGNORE);

    printf("PASS: UTF-8 spell reader pages (%d assertions)\n", checks);
    return 0;
}
