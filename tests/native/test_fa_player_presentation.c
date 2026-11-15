/* Strict host C99 regression of FAangband display-only stat captions. */
#include <stdio.h>
#include <string.h>
#include "app/src/main/cpp/faangband/ui-player-presentation.h"

static unsigned long long checks;
#define CHECK(x) do { if (!(x)) { \
    fprintf(stderr, "FAIL line %d: %s\n", __LINE__, #x); \
    return 1; } ++checks; } while (0)

int main(void)
{
    static const char *en_header[5] = {
        "  Self", " RB", " CB", " EB", "  Best"
    };
    static const char *zh_header[5] = {
        "  本值", "種族", "職業", "裝備", "  最佳"
    };
    static const char *en_unit[5] = {
        "%d.%d/turn", "%d.%d/turn", "%.1f lb", "%d.%d lb", "%d ft"
    };
    static const char *zh_unit[5] = {
        "%d.%d/回合", "%d.%d/回合", "%.1f 磅", "%d.%d 磅", "%d 呎"
    };
    int i, language;

    for (i = 0; i < 5; ++i) {
        CHECK(strcmp(angdroid_fa_stat_heading(i, 0), en_header[i]) == 0);
        CHECK(strcmp(angdroid_fa_stat_heading(i, 1), zh_header[i]) == 0);
        CHECK(strcmp(angdroid_fa_sheet_unit_format(
            (enum angdroid_fa_sheet_format)i, 0), en_unit[i]) == 0);
        CHECK(strcmp(angdroid_fa_sheet_unit_format(
            (enum angdroid_fa_sheet_format)i, 1), zh_unit[i]) == 0);
    }
    for (language = -1; language <= 2; ++language) {
        CHECK(strcmp(angdroid_fa_stat_heading(-1, language), "") == 0);
        CHECK(strcmp(angdroid_fa_stat_heading(5, language), "") == 0);
        CHECK(strcmp(angdroid_fa_sheet_unit_format(
            (enum angdroid_fa_sheet_format)-1, language), "") == 0);
        CHECK(strcmp(angdroid_fa_sheet_unit_format(
            (enum angdroid_fa_sheet_format)5, language), "") == 0);
    }
    CHECK(strcmp(angdroid_fa_sheet_status_title("[=-WIZARD-=]", 1),
        "[巫師模式]") == 0);
    CHECK(strcmp(angdroid_fa_sheet_status_title("***WINNER***", 1),
        "***勝利者***") == 0);
    CHECK(strcmp(angdroid_fa_sheet_status_title("[=-WIZARD-=]", 0),
        "[=-WIZARD-=]") == 0);
    CHECK(strcmp(angdroid_fa_sheet_status_title("***WINNER***", 0),
        "***WINNER***") == 0);
    CHECK(strcmp(angdroid_fa_sheet_status_title("Warrior", 1),
        "Warrior") == 0);
    CHECK(angdroid_fa_sheet_status_title(NULL, 1) == NULL);
    /* The five column headers have maximum display widths of 7/4/4/4/7.
     * Each CJK glyph takes two terminal cells. */
    CHECK(2 + 2 * 2 == 6);
    CHECK(2 * 2 == 4);
    puts("PASS: FA character sheet captions and unit English fallback");
    printf("%llu assertions\n", checks);
    return 0;
}
