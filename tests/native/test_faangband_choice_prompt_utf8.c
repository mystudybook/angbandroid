/* FAangband generic option prompts: UTF-8 cells, suffix and tight buffers. */
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include "../../app/src/main/cpp/faangband/ui-confirm-prompt-utf8.h"

static unsigned long checks;
#define CHECK(x) do { if (!(x)) { \
    fprintf(stderr, "FAIL line %d: %s\n", __LINE__, #x); \
    return 1; } ++checks; } while (0)

static int test_all_term_widths_and_buffers(void)
{
    static const char *questions[] = {
        "", "Learn a specialty now?", "繼續學習武器專精，確定嗎？",
        "漢字與😀 emoji 混合顯示", "ABC\xff\x64\x65\x66"
    };
    static const char *choices[] = {"a", "yns", "abcde", "", NULL};
    size_t qi, ci, cap;
    int width;
    for (qi = 0; qi < sizeof(questions) / sizeof(questions[0]); ++qi) {
        for (ci = 0; ci < sizeof(choices) / sizeof(choices[0]); ++ci) {
            for (width = -2; width <= 110; ++width) {
                for (cap = 0; cap <= 120; ++cap) {
                    char out[124];
                    size_t cells, limit;
                    memset(out, 'Z', sizeof(out));
                    cells = angdroid_fa_choice_prompt_line(
                        out, cap, questions[qi], choices[ci], width);
                    CHECK(out[120] == 'Z' && out[123] == 'Z');
                    if (cap == 0) {
                        CHECK(out[0] == 'Z' && cells == 0);
                        continue;
                    }
                    CHECK(memchr(out, '\0', cap) != NULL);
                    CHECK(out[cap] == 'Z');
                    CHECK(cells == angdroid_utf8_display_columns(out));
                    limit = width <= 0 ? 0 : (size_t)width;
                    CHECK(cells <= limit);
                    if (cap > 1 && choices[ci] && choices[ci][0] &&
                        limit >= strlen(choices[ci]) + 3 &&
                        cap > strlen(choices[ci]) + 3) {
                        size_t n = strlen(out);
                        size_t suffix = strlen(choices[ci]) + 3;
                        CHECK(n >= suffix);
                        CHECK(out[n - suffix] == '[');
                        CHECK(memcmp(out + n - suffix + 1, choices[ci],
                            strlen(choices[ci])) == 0);
                        CHECK(out[n - 2] == ']' && out[n - 1] == ' ');
                    }
                    if (limit <= 3 && cap > 1 && choices[ci] &&
                        choices[ci][0]) {
                        CHECK(out[0] == '?' || !out[0]);
                    }
                }
            }
        }
    }
    return 0;
}

static int test_chinese_and_compact_option_label(void)
{
    char long_name[1024], out[512];
    size_t i;
    for (i = 0; i < 250; ++i) memcpy(long_name + i * 3, "甲", 3);
    long_name[750] = '\0';

    CHECK(angdroid_fa_choice_prompt_line(out, sizeof(out),
        long_name, "yns", 17) <= 17);
    CHECK(strstr(out, "[yns] ") != NULL);
    CHECK(angdroid_utf8_display_columns(out) <= 17);
    CHECK(angdroid_fa_choice_prompt_line(out, sizeof(out),
        "你好", "abcdef", 4) == 4);
    CHECK(strcmp(out, "[a+]") == 0);
    CHECK(angdroid_fa_choice_prompt_line(out, sizeof(out),
        long_name, "yns", 3) == 1);
    CHECK(strcmp(out, "?") == 0);
    CHECK(angdroid_fa_choice_prompt_line(out, 7,
        long_name, "yns", 200) == 6);
    CHECK(strcmp(out, "[yns] ") == 0);
    CHECK(angdroid_fa_choice_prompt_line(out, sizeof(out),
        long_name, NULL, 6) <= 6);
    CHECK(angdroid_utf8_display_columns(out) <= 6);
    CHECK(angdroid_fa_choice_prompt_line(NULL, sizeof(out),
        long_name, "yns", 20) == 0);
    return 0;
}

int main(void)
{
    if (test_all_term_widths_and_buffers() ||
        test_chinese_and_compact_option_label()) return 1;
    printf("PASS: %lu FA generic choice prompt width/capacity checks\n",
        checks);
    return 0;
}
