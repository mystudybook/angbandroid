/* Host-only C99 test of the production FAangband spell layout. */
#include <limits.h>
#include <stdio.h>
#include "app/src/main/cpp/faangband/ui-spell-viewport.h"

static unsigned long long checks;
#define CHECK(c) do { if (!(c)) { \
    fprintf(stderr, "FAIL line %d: %s\n", __LINE__, #c); \
    return 1; } ++checks; } while (0)

int main(void)
{
    int width, height, count;
    CHECK(angdroid_fa_spell_menu_columns(INT_MIN) == 0);
    CHECK(angdroid_fa_spell_menu_columns(INT_MAX) == 80);
    CHECK(angdroid_fa_spell_menu_rows(INT_MAX, INT_MAX) > 0);
    CHECK(angdroid_fa_spell_menu_rows(4, INT_MAX) == 2);
    CHECK(angdroid_fa_spell_menu_rows(3, 10) == 0);
    CHECK(angdroid_fa_spell_menu_rows(10, 0) == 0);
    for (width = 0; width <= 160; ++width) {
        int cells = angdroid_fa_spell_menu_columns(width);
        CHECK(cells == (width < 8 ? 0 : (width < 80 ? width : 80)));
        if (cells) CHECK(cells <= width);
    }
    for (height = 0; height <= 180; ++height) {
        for (count = -1; count <= 220; ++count) {
            int rows = angdroid_fa_spell_menu_rows(height, count);
            CHECK(rows >= 0);
            if (height < 4 || count <= 0) {
                CHECK(rows == 0);
            } else {
                int reserve = height / 3;
                int maximum;
                if (reserve < 1) reserve = 1;
                if (reserve > 5) reserve = 5;
                maximum = height - 1 - reserve;
                if (maximum < 2) maximum = 2;
                CHECK(rows == (count >= maximum - 1 ?
                    maximum : count + 1));
                CHECK(rows >= 2);
                CHECK(rows <= height - 2);
                /* Header eats one row, the description begins below the
                 * list at physical line 1 + rows (zero-based). */
                CHECK(1 + rows < height);
                CHECK(rows - 1 <= count);
            }
        }
    }
    puts("PASS: FAangband spell menu geometry");
    printf("%llu assertions\n", checks);
    return 0;
}
