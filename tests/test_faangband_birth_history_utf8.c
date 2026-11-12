/* Host-buildable native regression for the FAangband character-history
 * editor. There is no dependency on game data, NDK, or Android runtime. */
#include <stdio.h>
#include <string.h>
#include "../app/src/main/cpp/faangband/ui-birth-history-utf8.h"
#include "../app/src/main/cpp/faangband/ui-ability-compact-layout.h"

static int expect(const char *s, size_t width, size_t expected,
        size_t *want_starts, size_t *want_lengths)
{
    size_t starts[256] = {0}, lengths[256] = {0};
    size_t n = angdroid_fa_history_wrap(s, width,
        starts, lengths, 256);
    size_t i;
    if (n != expected) {
        fprintf(stderr, "wrap count wrong: got %zu expected %zu\n",
            n, expected);
        return 1;
    }
    for (i = 0; i < n; ++i) {
        char line[256];
        angdroid_fa_history_copy_line(line, sizeof(line),
            s, starts[i], lengths[i]);
        if (starts[i] != want_starts[i] ||
                lengths[i] != want_lengths[i]) {
            fprintf(stderr, "line %zu wrong: %zu/%zu vs %zu/%zu\n",
                i, starts[i], lengths[i],
                want_starts[i], want_lengths[i]);
            return 1;
        }
        if (angdroid_utf8_display_columns(line) > width && width > 1) {
            fprintf(stderr, "line %zu exceeds terminal width %zu\n",
                i, width);
            return 1;
        }
    }
    return 0;
}

int main(void)
{
    size_t empty_s[] = {0}, empty_n[] = {0};
    size_t ascii_s[] = {0, 3}, ascii_n[] = {3, 3};
    size_t cjk_s[] = {0, 2}, cjk_n[] = {2, 2};
    size_t mixed_s[] = {0, 4}, mixed_n[] = {4, 2};
    size_t new_s[] = {0, 3}, new_n[] = {2, 2};
    size_t actual_start[1], actual_length[1];
    char line[32];

    if (expect("", 10, 1, empty_s, empty_n)) return 1;
    if (expect("abcdef", 3, 2, ascii_s, ascii_n)) return 1;
    if (expect("中文測試", 4, 2, cjk_s, cjk_n)) return 1;
    if (expect("中文ABCD", 6, 2, mixed_s, mixed_n)) return 1;
    if (expect("甲A\n乙B", 4, 2, new_s, new_n)) return 1;

    if (angdroid_fa_history_wrap("中文測試", 4,
            actual_start, actual_length, 1) != 1)
        return 1;
    if (actual_start[0] != 0 || actual_length[0] != 2)
        return 1;

    if (angdroid_fa_history_copy_line(line, sizeof(line),
            "甲乙ABC", 0, 2) != strlen("甲乙"))
        return 1;
    if (strcmp(line, "甲乙")) return 1;
    if (angdroid_fa_history_copy_line(line, sizeof(line),
            "\xffZ", 0, 2) != 2 || strcmp(line, "?Z"))
        return 1;
    if (angdroid_fa_history_copy_line(line, 4,
            "甲乙", 0, 2) != 3 || strcmp(line, "甲"))
        return 1;

    /* Regression: the ability browser must be able to show hundreds of
     * explicit Han lines instead of losing everything beyond line 256. */
    {
        char long_text[1400];
        char last[16];
        size_t starts[1400], lengths[1400], count, cap;
        size_t i;
        for (i = 0; i < 320; ++i)
            memcpy(long_text + 4 * i, "甲\n", 4);
        memcpy(long_text + 1280, "乙", 4);
        cap = angdroid_fa_ability_line_capacity(strlen(long_text));
        if (cap != 1284) return 1;
        count = angdroid_fa_history_wrap(long_text, 16,
            starts, lengths, cap);
        if (count != 321 || starts[320] != 640 ||
            lengths[320] != 1) return 1;
        angdroid_fa_history_copy_line(last, sizeof(last),
            long_text, starts[320], lengths[320]);
        if (strcmp(last, "乙")) return 1;
    }

    puts("PASS: FA history wrap: ASCII, Han, mixed, newline, 321 lines");
    return 0;
}
