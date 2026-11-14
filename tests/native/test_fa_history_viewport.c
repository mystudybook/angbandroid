/* Strict host C99 test of the exact production FAangband history viewport. */
#include <limits.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include "app/src/main/cpp/faangband/ui-history-viewport.h"

static unsigned long long checks;
#define CHECK(expr) do { if (!(expr)) { \
    fprintf(stderr, "FAIL line %d: %s\n", __LINE__, #expr); \
    return 1; } ++checks; } while (0)

int main(void)
{
    int height, rows, count, top, step;
    size_t expected;

    CHECK(angdroid_fa_history_page_rows(-100) == 0);
    CHECK(angdroid_fa_history_page_rows(0) == 0);
    CHECK(angdroid_fa_history_page_rows(3) == 0);
    CHECK(angdroid_fa_history_page_rows(4) == 1);
    CHECK(angdroid_fa_history_page_rows(24) == 21);
    CHECK(angdroid_fa_history_page_rows(INT_MAX) == INT_MAX - 3);
    CHECK(angdroid_fa_history_clamp_top(SIZE_MAX, 0) == 0);
    CHECK(angdroid_fa_history_clamp_top(SIZE_MAX, 2) == 1);
    CHECK(angdroid_fa_history_clamp_top(0, 20) == 0);
    CHECK(angdroid_fa_history_step(SIZE_MAX, 0, INT_MAX, 1) == 0);
    CHECK(angdroid_fa_history_step(SIZE_MAX, SIZE_MAX, 20, 1)
        == SIZE_MAX - 1);
    CHECK(angdroid_fa_history_step(SIZE_MAX, SIZE_MAX, INT_MAX, 0)
        == SIZE_MAX - 1 - (size_t)INT_MAX);
    CHECK(angdroid_fa_history_step(0, SIZE_MAX, INT_MAX, 1)
        == (size_t)INT_MAX);
    CHECK(angdroid_fa_history_step(0, SIZE_MAX, INT_MAX, 0) == 0);

    for (height = 0; height <= 128; ++height) {
        rows = angdroid_fa_history_page_rows(height);
        CHECK(rows == (height > 3 ? height - 3 : 0));
        if (rows > 0) CHECK(2 + rows - 1 < height - 1);
    }
    for (count = 0; count <= 60; ++count) {
        for (top = 0; top <= count + 5; ++top) {
            size_t safe_top = count == 0 ? 0 :
                ((size_t)top < (size_t)count ? (size_t)top :
                (size_t)(count - 1));
            CHECK(angdroid_fa_history_clamp_top((size_t)top,
                (size_t)count) == safe_top);
            for (step = -1; step <= 80; ++step) {
                int direction;
                for (direction = 0; direction <= 1; ++direction) {
                    expected = safe_top;
                    if (count && step > 0) {
                        if (direction) {
                            size_t remaining =
                                (size_t)(count - 1) - safe_top;
                            expected = (size_t)step >= remaining ?
                                (size_t)count - 1 :
                                safe_top + (size_t)step;
                        } else {
                            expected = (size_t)step >= safe_top ? 0 :
                                safe_top - (size_t)step;
                        }
                    }
                    CHECK(angdroid_fa_history_step((size_t)top,
                        (size_t)count, step, direction) == expected);
                }
            }
        }
    }

    printf("PASS: %llu FA history viewport C99 checks\n", checks);
    return 0;
}
