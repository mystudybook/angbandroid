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
    const char *spell_header_en = angdroid_spell_menu_header(0);
    const char *spell_header_zh = angdroid_spell_menu_header(1);

    /* Display-only zh-TW header: 50 cells with the same numeric columns. */
    CHECK(strcmp(spell_header_en,
        "Name                             Lv Mana Fail Info") == 0);
    CHECK(strcmp(spell_header_zh,
        "法術名稱                         等 魔力 失敗 資訊") == 0);
    CHECK(angdroid_utf8_display_columns(spell_header_en) == 50);
    CHECK(angdroid_utf8_display_columns(spell_header_zh) == 50);
    CHECK(angdroid_utf8_display_columns("法術名稱                         ") == 33);
    CHECK(angdroid_utf8_display_columns("法術名稱                         等 ") == 36);
    CHECK(angdroid_utf8_display_columns("法術名稱                         等 魔力 ") == 41);
    CHECK(angdroid_utf8_display_columns("法術名稱                         等 魔力 失敗 ") == 46);

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

    /* The selected spell book must get a fresh, bounded menu region when
     * returning from a full reader after portrait/landscape rotation. */
    CHECK(angdroid_spell_reader_menu_width(-1) == 0);
    CHECK(angdroid_spell_reader_menu_width(0) == 0);
    CHECK(angdroid_spell_reader_menu_width(4) == 0);
    CHECK(angdroid_spell_reader_menu_width(15) == 0);
    CHECK(angdroid_spell_reader_menu_width(16) == 1);
    CHECK(angdroid_spell_reader_menu_width(20) == 5);
    CHECK(angdroid_spell_reader_menu_width(40) == 25);
    CHECK(angdroid_spell_reader_menu_width(94) == 79);
    CHECK(angdroid_spell_reader_menu_width(95) == 80);
    CHECK(angdroid_spell_reader_menu_width(96) == 80);
    CHECK(angdroid_spell_reader_menu_width(500) == 80);
    /* Recomputing for a new width must not be based on the prior width. */
    CHECK(angdroid_spell_reader_menu_width(40) == 25);
    CHECK(angdroid_spell_reader_menu_width(95) == 80);
    CHECK(angdroid_spell_reader_menu_width(20) == 5);
    CHECK(angdroid_spell_reader_menu_width(40) == 25);

    /* The global menu dispatches an active row click as the raw mouse
     * event, after updating its cursor; the spell handler must accept it.
     * Header/outside taps and right-click/back must not select a spell. */
    CHECK(angdroid_spell_menu_touch_selectable(1, 0, 0));
    CHECK(angdroid_spell_menu_touch_selectable(1, 1, 0));
    CHECK(angdroid_spell_menu_touch_selectable(1, 0, 6));
    CHECK(angdroid_spell_menu_touch_selectable(1, 1, 23));
    CHECK(!angdroid_spell_menu_touch_selectable(0, 0, 0));
    CHECK(!angdroid_spell_menu_touch_selectable(0, 1, 4));
    CHECK(!angdroid_spell_menu_touch_selectable(0, 2, 5));
    CHECK(!angdroid_spell_menu_touch_selectable(1, 2, 5));
    CHECK(!angdroid_spell_menu_touch_selectable(1, 1, -1));
    CHECK(!angdroid_spell_menu_touch_selectable(1, 0, -1));
    CHECK(!angdroid_spell_menu_touch_selectable(0, 0, -1));
    CHECK(angdroid_spell_menu_touch_selectable(1, 0, 0));

    /* Two taps on the SAME spell are valid only within two monotonic
     * seconds. A stale tap is a fresh FIRST tap, never a cast. */
    {
        int armed = -1;
        uint64_t at = 0;
        CHECK(ANGDROID_SPELL_TOUCH_CONFIRM_WINDOW_MS == UINT64_C(2000));
        CHECK(!angdroid_spell_menu_confirm_touch(
            &armed, &at, 0, 0, UINT64_C(1000)));
        CHECK(armed == 0 && at == UINT64_C(1000));
        CHECK(angdroid_spell_menu_confirm_touch(
            &armed, &at, 0, 0, UINT64_C(3000)));
        CHECK(armed == -1 && at == 0);
        /* Two callbacks in the very same millisecond must not appear
         * to be two separate intentional taps. */
        CHECK(!angdroid_spell_menu_confirm_touch(
            &armed, &at, 0, 0, UINT64_C(3500)));
        CHECK(!angdroid_spell_menu_confirm_touch(
            &armed, &at, 0, 0, UINT64_C(3500)));
        CHECK(armed == 0 && at == UINT64_C(3500));
        CHECK(angdroid_spell_menu_confirm_touch(
            &armed, &at, 0, 0, UINT64_C(3501)));
        CHECK(armed == -1 && at == 0);
        CHECK(!angdroid_spell_menu_confirm_touch(
            &armed, &at, 0, 0, UINT64_C(4000)));
        CHECK(armed == 0 && at == UINT64_C(4000));
        CHECK(!angdroid_spell_menu_confirm_touch(
            &armed, &at, 0, 0, UINT64_C(6001)));
        CHECK(armed == 0 && at == UINT64_C(6001));
        CHECK(angdroid_spell_menu_confirm_touch(
            &armed, &at, 0, 0, UINT64_C(6002)));
        CHECK(armed == -1 && at == 0);
    }

    /* Switching rows resets the deadline. Stale taps on the new row
     * must also rearm, not cast. */
    {
        int armed = -1;
        uint64_t at = 0;
        CHECK(!angdroid_spell_menu_confirm_touch(
            &armed, &at, 3, 0, UINT64_C(1000)));
        CHECK(!angdroid_spell_menu_confirm_touch(
            &armed, &at, 7, 0, UINT64_C(1100)));
        CHECK(armed == 7 && at == UINT64_C(1100));
        CHECK(!angdroid_spell_menu_confirm_touch(
            &armed, &at, 7, 0, UINT64_C(4000)));
        CHECK(armed == 7 && at == UINT64_C(4000));
        CHECK(angdroid_spell_menu_confirm_touch(
            &armed, &at, 7, 0, UINT64_C(4400)));
        CHECK(armed == -1 && at == 0);
    }

    /* Monotonic time may fail, roll back, or approach UINT64_MAX.
     * None of those should permit an incorrect confirmation. */
    {
        int armed = -1;
        uint64_t at = 0;
        CHECK(!angdroid_spell_menu_confirm_touch(
            &armed, &at, 2, 0, 0));
        CHECK(armed == 2 && at == 0);
        CHECK(!angdroid_spell_menu_confirm_touch(
            &armed, &at, 2, 0, 0));
        CHECK(!angdroid_spell_menu_confirm_touch(
            &armed, &at, 2, 0, UINT64_C(5000)));
        CHECK(armed == 2 && at == UINT64_C(5000));
        CHECK(!angdroid_spell_menu_confirm_touch(
            &armed, &at, 2, 0, UINT64_C(4999)));
        CHECK(armed == 2 && at == UINT64_C(4999));
        CHECK(angdroid_spell_menu_confirm_touch(
            &armed, &at, 2, 0, UINT64_C(5000)));
        CHECK(armed == -1 && at == 0);
        CHECK(!angdroid_spell_menu_confirm_touch(
            &armed, &at, 5, 0, UINT64_MAX - UINT64_C(800)));
        CHECK(angdroid_spell_menu_confirm_touch(
            &armed, &at, 5, 0, UINT64_MAX));
        CHECK(armed == -1 && at == 0);
    }

    /* Browsing and keyboard interaction remain independent of touch
     * confirmation: browsing needs one tap and clears any old timer. */
    {
        int armed = 8;
        uint64_t at = UINT64_C(2000);
        CHECK(angdroid_spell_menu_confirm_touch(
            &armed, &at, 3, 1, UINT64_C(9000)));
        CHECK(armed == -1 && at == 0);
        CHECK(angdroid_spell_menu_confirm_touch(
            &armed, &at, 3, 1, UINT64_C(9001)));
        CHECK(armed == -1 && at == 0);
        CHECK(!angdroid_spell_menu_confirm_touch(
            NULL, &at, 4, 0, UINT64_C(10000)));
        CHECK(!angdroid_spell_menu_confirm_touch(
            &armed, NULL, 4, 0, UINT64_C(10000)));
        CHECK(!angdroid_spell_menu_confirm_touch(
            &armed, &at, -1, 0, UINT64_C(10000)));
        CHECK(armed == -1 && at == 0);
    }

    /* Screen resize, reader exit, a keyboard command, and a tapped
     * invalid row all clear BOTH the pending row and its timestamp. */
    {
        int armed = -1;
        uint64_t at = 0;
        CHECK(!angdroid_spell_menu_confirm_touch(
            &armed, &at, 5, 0, UINT64_C(1200)));
        CHECK(armed == 5 && at == UINT64_C(1200));
        angdroid_spell_menu_cancel_touch(&armed, &at);
        CHECK(armed == -1 && at == 0);
        CHECK(!angdroid_spell_menu_confirm_touch(
            &armed, &at, 5, 0, UINT64_C(1300)));
        CHECK(armed == 5 && at == UINT64_C(1300));
        angdroid_spell_menu_cancel_touch(&armed, &at);
        CHECK(!angdroid_spell_menu_confirm_touch(
            &armed, &at, 5, 0, UINT64_C(1400)));
        CHECK(armed == 5 && at == UINT64_C(1400));
        angdroid_spell_menu_cancel_touch(&armed, &at);
        CHECK(!angdroid_spell_menu_confirm_touch(
            &armed, &at, 5, 0, UINT64_C(1500)));
        CHECK(armed == 5 && at == UINT64_C(1500));
        angdroid_spell_menu_cancel_touch(&armed, &at);
        CHECK(armed == -1 && at == 0);
        angdroid_spell_menu_cancel_touch(NULL, &at);
        angdroid_spell_menu_cancel_touch(&armed, NULL);
        CHECK(armed == -1 && at == 0);
    }

    /* Lifecycle changes invalidate a native first tap even when only a
     * few milliseconds elapsed and the KeyBuffer was already cleared. */
    {
        int armed = -1, known_epoch = 4;
        uint64_t at = 0;

        CHECK(!angdroid_spell_menu_touch_epoch_changed(
            &armed, &at, &known_epoch, 4));
        CHECK(!angdroid_spell_menu_confirm_touch(
            &armed, &at, 3, 0, UINT64_C(1000)));
        CHECK(armed == 3 && at == UINT64_C(1000));

        CHECK(angdroid_spell_menu_touch_epoch_changed(
            &armed, &at, &known_epoch, 5)); /* onPause */
        CHECK(known_epoch == 5 && armed == -1 && at == 0);
        CHECK(!angdroid_spell_menu_confirm_touch(
            &armed, &at, 3, 0, UINT64_C(1050)));
        CHECK(armed == 3 && at == UINT64_C(1050));
        CHECK(angdroid_spell_menu_touch_epoch_changed(
            &armed, &at, &known_epoch, 6)); /* onResume */
        CHECK(known_epoch == 6 && armed == -1 && at == 0);
        CHECK(!angdroid_spell_menu_confirm_touch(
            &armed, &at, 3, 0, UINT64_C(1100)));
        CHECK(armed == 3 && at == UINT64_C(1100));
        CHECK(angdroid_spell_menu_confirm_touch(
            &armed, &at, 3, 0, UINT64_C(1400)));
        CHECK(armed == -1 && at == 0);

        /* A repeated poll with an unchanged generation has no effect. */
        CHECK(!angdroid_spell_menu_touch_epoch_changed(
            &armed, &at, &known_epoch, 6));
        CHECK(known_epoch == 6);
    }

    /* Transition arriving between confirmation and assignment: reject
     * the just-confirmed second tap, even if the first poll was unchanged. */
    {
        int armed = -1, known_epoch = 40;
        uint64_t at = 0;
        CHECK(!angdroid_spell_menu_touch_epoch_changed(
            &armed, &at, &known_epoch, 40));
        CHECK(!angdroid_spell_menu_confirm_touch(
            &armed, &at, 3, 0, UINT64_C(1000)));
        CHECK(!angdroid_spell_menu_touch_epoch_changed(
            &armed, &at, &known_epoch, 40));
        CHECK(angdroid_spell_menu_confirm_touch(
            &armed, &at, 3, 0, UINT64_C(1100)));
        CHECK(armed == -1 && at == 0);
        CHECK(angdroid_spell_menu_touch_epoch_changed(
            &armed, &at, &known_epoch, 41)); /* onPause races with tap */
        CHECK(known_epoch == 41 && armed == -1 && at == 0);
        CHECK(!angdroid_spell_menu_confirm_touch(
            &armed, &at, 3, 0, UINT64_C(1200)));
        CHECK(armed == 3 && at == UINT64_C(1200));
        CHECK(angdroid_spell_menu_confirm_touch(
            &armed, &at, 3, 0, UINT64_C(1300)));
    }

    /* A JNI failure must fail closed; no pending spell may survive. */
    {
        int armed = 8, known_epoch = 10;
        uint64_t at = UINT64_C(9000);
        CHECK(angdroid_spell_menu_touch_epoch_changed(
            &armed, &at, &known_epoch, -1));
        CHECK(armed == -1 && at == 0 && known_epoch == -1);
        CHECK(angdroid_spell_menu_touch_epoch_changed(
            &armed, &at, &known_epoch, 0));
        CHECK(armed == -1 && at == 0 && known_epoch == -1);
        CHECK(angdroid_spell_menu_touch_epoch_changed(
            &armed, &at, &known_epoch, 11));
        CHECK(armed == -1 && at == 0 && known_epoch == 11);
        CHECK(!angdroid_spell_menu_touch_epoch_changed(
            &armed, &at, &known_epoch, 11));
        CHECK(angdroid_spell_menu_touch_epoch_changed(
            NULL, &at, &known_epoch, 12));
        CHECK(angdroid_spell_menu_touch_epoch_changed(
            &armed, NULL, &known_epoch, 12));
        CHECK(angdroid_spell_menu_touch_epoch_changed(
            &armed, &at, NULL, 12));
    }

    printf("PASS: UTF-8 spell reader pages (%d assertions)\n", checks);
    return 0;
}
