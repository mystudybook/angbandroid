/* Host C99: FAangband confirmation prompts reserve [y/n] and whole UTF-8. */
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include "../../app/src/main/cpp/faangband/ui-confirm-prompt-utf8.h"

static unsigned long checks;
#define CHECK(x) do { if (!(x)) { \
    fprintf(stderr, "FAIL line %d: %s\n", __LINE__, #x); \
    return 1; } ++checks; } while (0)

static int test_all_widths(void)
{
    static const char *texts[] = {
        "", "Very long English specialty description follows",
        "護甲精通，確定學習？", "中文測試甲乙丙丁戊己庚辛壬癸",
        "訓練😀中文與emoji\xff測試"
    };
    int width;
    size_t cap, t;
    for (t = 0; t < sizeof(texts) / sizeof(texts[0]); ++t) {
        for (width = -2; width < 96; ++width) {
            for (cap = 0; cap <= 128; ++cap) {
                char buffer[130];
                size_t cells, byte_count, drawable;
                memset(buffer, 0x5a, sizeof(buffer));
                cells = angdroid_fa_confirm_prompt_line(
                    buffer, cap, texts[t], width);
                CHECK(buffer[128] == 'Z');
                CHECK(buffer[129] == 'Z');
                if (!cap) {
                    CHECK(buffer[0] == 'Z' && cells == 0);
                    continue;
                }
                CHECK(memchr(buffer, '\0', cap) != NULL);
                byte_count = strlen(buffer);
                CHECK(byte_count < cap);
                CHECK(cells == angdroid_utf8_display_columns(buffer));
                drawable = width > 0 ? (size_t)width : 0;
                CHECK(cells <= drawable);
                if (!drawable) {
                    CHECK(buffer[0] == '\0');
                    continue;
                }
                if (drawable >= 6 && cap >= 7) {
                    CHECK(byte_count >= 6);
                    CHECK(strcmp(buffer + byte_count - 6, "[y/n] ") == 0);
                }
                if (drawable > 0 && drawable <= 5 && cap > 1) {
                    CHECK(strstr(buffer, "[y/n") == NULL);
                }
            }
        }
    }
    return 0;
}

static int test_long_chinese_and_byte_boundaries(void)
{
    char src[1024], output[512];
    size_t i;
    for (i = 0; i < 200; ++i)
        memcpy(src + i * 3, "甲", 3);
    src[600] = '\0';
    CHECK(angdroid_fa_confirm_prompt_line(output,
        sizeof(output), src, 16) <= 16);
    CHECK(angdroid_utf8_display_columns(output) <= 16);
    CHECK(strstr(output, "[y/n] ") != NULL);
    CHECK(strcmp(output + strlen(output) - 6, "[y/n] ") == 0);
    CHECK(angdroid_fa_confirm_prompt_line(output,
        7, src, 16) == 6);
    CHECK(strcmp(output, "[y/n] ") == 0);
    CHECK(angdroid_fa_confirm_prompt_line(output,
        sizeof(output), NULL, 15) == 6);
    CHECK(strcmp(output, "[y/n] ") == 0);
    CHECK(angdroid_fa_confirm_prompt_line(NULL, 10, src, 20) == 0);
    return 0;
}

int main(void)
{
    if (test_all_widths() || test_long_chinese_and_byte_boundaries())
        return 1;
    printf("PASS: %lu FA confirmation UTF-8 width and capacity checks\n",
        checks);
    return 0;
}
