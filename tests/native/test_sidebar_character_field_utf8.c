/* Host C99: character sidebar names stay within fixed terminal columns.
 * Tests the same menu-text-fit helper used by ui-display.c in both engines.
 */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/menu-text-fit.h"
#include "../../app/src/main/cpp/angband/zh-tw-ui.h"
#include "../../app/src/main/cpp/angband/zh-tw-class-titles.h"

static unsigned checks;
#define CHECK(expr) do { assert(expr); ++checks; } while (0)
static void fit(const char *input, size_t width, const char *expected,
                size_t expected_columns)
{
    char text[96];
    size_t cells = angdroid_menu_copy_columns(text, sizeof(text),
                                               input, width);
    CHECK(cells == expected_columns);
    CHECK(strcmp(text, expected) == 0);
    CHECK(angdroid_utf8_display_columns(text) == expected_columns);
    CHECK(cells <= width);
}

int main(void)
{
    const char *english = "Arch-Mage";
    const char *long_title = "高階神秘學者高階神秘學者";
    const char *unknown = "A new class title";

    fit("人類", 13, "人類", 4);
    fit("高等精靈", 13, "高等精靈", 8);
    fit("人類戰士", 13, "人類戰士", 8);
    fit(long_title, 13, "高階神秘學者", 12);
    fit("高等精靈", 7, "高等精", 6);
    fit("高等精靈", 1, "", 0);
    fit("A中B", 3, "A中", 3);
    fit("A中B", 2, "A", 1);
    fit("0123456789ABCDEF", 13, "0123456789ABC", 13);
    fit(NULL, 13, "", 0);
    fit("人類", 0, "", 0);

    /* The original English strings are returned without modification. */
    CHECK(angdroid_zh_tw_class_title(english, 0) == english);
    CHECK(strcmp(angdroid_zh_tw_class_title(english, 1), "大法師") == 0);
    CHECK(angdroid_zh_tw_class_title(unknown, 1) == unknown);
    CHECK(strcmp(angdroid_game_ui_text("Human", 1), "人類") == 0);
    CHECK(strcmp(angdroid_game_ui_text("Warrior", 1), "戰士") == 0);
    CHECK(strcmp(angdroid_game_ui_text("High-Elf", 1), "高等精靈") == 0);
    CHECK(strcmp(angdroid_game_ui_text("Human", 0), "Human") == 0);

    puts("PASS: fixed 13-cell sidebar and compact UTF-8 glyph clipping");
    printf("%u character label checks passed\n", checks);
    return 0;
}
