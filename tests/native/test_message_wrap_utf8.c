/* Long top-line messages must wrap by display columns, not UTF-8 bytes. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/message-wrap-utf8.h"

#define CHECK(v) do { assert((v)); ++checks; } while (0)
static int checks;

static void check_round_trip(const char *s, size_t columns)
{
    char result[512] = { 0 };
    size_t used = 0;
    int steps = 0;
    const char *p = s;
    while (*p) {
        struct angdroid_message_piece part =
            angdroid_message_next_piece(p, columns);
        CHECK(part.bytes > 0 && part.advance >= part.bytes);
        CHECK(part.columns <= columns ||
              (columns == 1 && part.columns == 2));
        CHECK(used + part.advance < sizeof(result));
        memcpy(result + used, p, part.bytes);
        used += part.bytes;
        if (part.advance != part.bytes) result[used++] = ' ';
        p += part.advance;
        CHECK(++steps < 80);
        if (!part.more) CHECK(!*p);
    }
    CHECK(strcmp(result, s) == 0);
}

int main(void)
{
    struct angdroid_message_piece r;
    const char *mixed = "中文怪物測試 ABCDE 中文怪物測試";
    r = angdroid_message_next_piece("abcd efgh ijkl", 7);
    CHECK(r.bytes == 4 && r.columns == 4 && r.advance == 5 && r.more);
    r = angdroid_message_next_piece("中文測試ABC", 5);
    CHECK(r.bytes == 6 && r.columns == 4 && r.advance == 6 && r.more);
    r = angdroid_message_next_piece("中文測試ABC", 1);
    CHECK(r.bytes == 3 && r.columns == 2 && r.more);
    r = angdroid_message_next_piece("完成", 10);
    CHECK(!r.more && r.bytes == 6 && r.columns == 4 && r.advance == 6);
    r = angdroid_message_next_piece(NULL, 8);
    CHECK(!r.bytes && !r.more);
    r = angdroid_message_next_piece("test", 0);
    CHECK(!r.bytes && !r.more);

    check_round_trip("中文怪物法術說明內容完整。", 6);
    check_round_trip(mixed, 11);
    check_round_trip("ABCDEFGHIJKLMN", 5);
    check_round_trip("English words keep spaces and wrap near the last space", 9);
    check_round_trip("裝備 (屠龍) 效果 1234", 10);
    printf("PASS: UTF-8 message wrap (%d assertions)\n", checks);
    return 0;
}
