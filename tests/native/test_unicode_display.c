#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "../../app/src/main/cpp/curses/unicode_display.h"

static int cases = 0;
#define CHECK(condition) do { assert(condition); ++cases; } while (0)

int main(void)
{
    uint32_t cp = 0;
    char mixed[64];
    char too_wide[64];
    char invalid[] = { (char)0xE4, 'Z', '\0' };

    CHECK(angdroid_utf8_display_columns("") == 0);
    CHECK(angdroid_utf8_display_columns(NULL) == 0);
    CHECK(angdroid_utf8_display_columns("Ascii") == 5);
    CHECK(angdroid_utf8_display_columns("中文") == 4);
    CHECK(angdroid_utf8_display_columns("A中文B") == 6);
    CHECK(angdroid_utf8_display_columns("狗頭人") == 6);
    CHECK(angdroid_utf8_display_columns("é") == 1);
    CHECK(angdroid_utf8_display_columns(invalid) == 2);
    CHECK(angdroid_utf8_next_scalar("中", &cp) == 3 && cp == 0x4E2Du);
    CHECK(angdroid_utf8_next_scalar("", &cp) == 0);
    CHECK(angdroid_utf8_next_scalar(NULL, &cp) == 0);

    strcpy(mixed, "A中文B");
    angdroid_utf8_clip_columns(mixed, 0);
    CHECK(strcmp(mixed, "") == 0);
    strcpy(mixed, "A中文B");
    angdroid_utf8_clip_columns(mixed, 2);
    CHECK(strcmp(mixed, "A") == 0);
    strcpy(mixed, "A中文B");
    angdroid_utf8_clip_columns(mixed, 3);
    CHECK(strcmp(mixed, "A中") == 0);
    strcpy(mixed, "A中文B");
    angdroid_utf8_clip_columns(mixed, 5);
    CHECK(strcmp(mixed, "A中文") == 0);
    strcpy(mixed, "A中文B");
    angdroid_utf8_clip_columns(mixed, 6);
    CHECK(strcmp(mixed, "A中文B") == 0);
    strcpy(too_wide, "中文");
    angdroid_utf8_clip_columns(too_wide, 1);
    CHECK(strcmp(too_wide, "") == 0);
    strcpy(too_wide, "中文");
    angdroid_utf8_clip_columns(too_wide, 3);
    CHECK(strcmp(too_wide, "中") == 0);

    /* Message history horizontal scrolling must never split UTF-8. */
    CHECK(strcmp(angdroid_utf8_skip_columns("A中文B", 0), "A中文B") == 0);
    CHECK(strcmp(angdroid_utf8_skip_columns("A中文B", 1), "中文B") == 0);
    CHECK(strcmp(angdroid_utf8_skip_columns("A中文B", 2), "文B") == 0);
    CHECK(strcmp(angdroid_utf8_skip_columns("A中文B", 3), "文B") == 0);
    CHECK(strcmp(angdroid_utf8_skip_columns("A中文B", 5), "B") == 0);
    CHECK(strcmp(angdroid_utf8_skip_columns("A中文B", 999), "") == 0);
    CHECK(angdroid_utf8_skip_columns(NULL, 3) == NULL);
    {
        const char *mixed_text = "A中文B";
        const char *match = strstr(mixed_text, "文");
        CHECK(match != NULL);
        CHECK(angdroid_utf8_prefix_columns(mixed_text, match) == 3);
        CHECK(angdroid_utf8_prefix_columns(mixed_text, mixed_text) == 0);
    }

    CHECK(angdroid_utf8_printf_field_bytes("abc", 10) == 10);
    CHECK(angdroid_utf8_printf_field_bytes("中文", 6) == 8);
    CHECK(angdroid_utf8_printf_field_bytes("狗頭人", 10) == 13);
    CHECK(angdroid_utf8_printf_field_bytes("中文", 2) == strlen("中文"));
    CHECK(angdroid_utf8_printf_field_bytes(NULL, 5) == 5);


    /* Spell names must be UTF-8 valid BEFORE column clipping.  A byte
     * truncate may otherwise leave a dangling Han or emoji prefix. */
    {
        char dest[8];
        char tight[4];
        char tiny[2];
        char null_dest = 'Z';
        char broken[] = { (char)0xE4, 'Z', '\0' };

        CHECK(angdroid_utf8_copy_complete(dest, sizeof(dest), "甲乙丙")
              == strlen("甲乙"));
        CHECK(strcmp(dest, "甲乙") == 0);
        CHECK(angdroid_utf8_display_columns(dest) == 4);
        CHECK(angdroid_utf8_copy_complete(dest, sizeof(dest), "A中文")
              == strlen("A中"));
        CHECK(strcmp(dest, "A中") == 0);
        CHECK(angdroid_utf8_copy_complete(tight, sizeof(tight), "中文")
              == strlen("中"));
        CHECK(strcmp(tight, "中") == 0);
        CHECK(angdroid_utf8_copy_complete(tiny, sizeof(tiny), "中文") == 0);
        CHECK(tiny[0] == '\0');
        CHECK(angdroid_utf8_copy_complete(dest, sizeof(dest), "é🐱")
              == strlen("é🐱"));
        CHECK(strcmp(dest, "é🐱") == 0);
        CHECK(angdroid_utf8_copy_complete(dest, sizeof(dest), broken) == 2);
        CHECK(strcmp(dest, "?Z") == 0);
        CHECK(angdroid_utf8_copy_complete(dest, sizeof(dest), NULL) == 0);
        CHECK(dest[0] == '\0');
        CHECK(angdroid_utf8_copy_complete(NULL, 0, "中文") == 0);
        CHECK(angdroid_utf8_copy_complete(&null_dest, 0, "中文") == 0);
        CHECK(null_dest == 'Z');
        CHECK(angdroid_utf8_copy_complete(tiny, sizeof(tiny), "") == 0);
        CHECK(tiny[0] == '\0');
    }

    puts("PASS: UTF-8 terminal cell width, safe clipping and padding");
    printf("Assertions: %d\n", cases);
    return 0;
}
