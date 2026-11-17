/* Test the exact portable header used by native object descriptions. */
#include <assert.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/obj-desc-numeric-utf8.h"

static unsigned long assertions = 0;
#define CHECK(x) do { assert(x); ++assertions; } while (0)

static void examples(void)
{
    char out[192] = "長劍";
    size_t end = strlen(out);
    int modifiers[] = { 3, -2, INT_MIN, INT_MAX };
    CHECK(angdroid_obj_desc_append_formatted_detail(out, sizeof(out), &end,
        " (%+d,%+d)", 5, -3));
    CHECK(strcmp(out, "長劍 (+5,-3)") == 0);
    CHECK(angdroid_obj_desc_append_formatted_detail(out, sizeof(out), &end,
        " [%d,%+d]", 12, 4));
    CHECK(strcmp(out, "長劍 (+5,-3) [12,+4]") == 0);
    CHECK(angdroid_obj_desc_append_modifiers_ascii(out, sizeof(out), &end,
        modifiers, 4));
    CHECK(strcmp(out, "長劍 (+5,-3) [12,+4] <+3, -2, -2147483648, +2147483647>") == 0);
    CHECK(end == strlen(out));
    CHECK(angdroid_obj_desc_append_formatted_detail(out, sizeof(out), &end,
        " (%d charge%s)", 3, "s"));
    CHECK(strstr(out, " (3 charges)") != NULL);
    CHECK(angdroid_obj_desc_append_formatted_detail(out, sizeof(out), &end,
        " (charging)"));
    CHECK(strstr(out, " (charging)") != NULL);
    CHECK(angdroid_obj_desc_append_formatted_detail(out, sizeof(out), &end,
        "（剩餘 %d 回合）", INT_MAX));
    CHECK(strstr(out, "（剩餘 2147483647 回合）") != NULL);
    CHECK(!angdroid_obj_desc_append_formatted_detail(out, sizeof(out), &end,
        ""));
    CHECK(!angdroid_obj_desc_append_formatted_detail(NULL, sizeof(out), &end,
        " [%d]", 1));
    CHECK(!angdroid_obj_desc_append_formatted_detail(out, sizeof(out), NULL,
        " [%d]", 1));
    CHECK(!angdroid_obj_desc_append_formatted_detail(out, 0, &end,
        " [%d]", 1));
    CHECK(!angdroid_obj_desc_append_formatted_detail(out, sizeof(out), &end,
        NULL));
    CHECK(!angdroid_obj_desc_append_modifiers_ascii(out, sizeof(out), &end,
        NULL, 1));
    CHECK(!angdroid_obj_desc_append_modifiers_ascii(out, sizeof(out), &end,
        modifiers, 0));
    CHECK(end == strlen(out));
}

static void capacities(void)
{
    const int modifiers[] = { +12, -3, +4, INT_MIN };
    size_t cap, offset, mode;
    for (cap = 0; cap < 145; ++cap)
        for (offset = 0; offset < 17; ++offset)
            for (mode = 0; mode < 8; ++mode) {
                char memory[160], original[160], *out = memory + 1;
                size_t end = offset, expected_len = 0;
                const char *expected = NULL;
                bool success;
                memset(memory, '#', sizeof(memory));
                if (offset < cap) {
                    memset(out, 'X', offset);
                    out[offset] = 0;
                }
                memcpy(original, memory, sizeof(memory));
                switch (mode) {
                case 0:
                    expected = " (%dd%d)";
                    success = angdroid_obj_desc_append_formatted_detail(out,
                        cap, &end, " (%dd%d)", 2, 6);
                    expected_len = strlen(" (2d6)");
                    break;
                case 1:
                    expected = " (%+d,%+d)";
                    success = angdroid_obj_desc_append_formatted_detail(out,
                        cap, &end, " (%+d,%+d)", INT_MIN, INT_MAX);
                    expected_len = strlen(" (-2147483648,+2147483647)");
                    break;
                case 2:
                    expected = " [%d,%+d]";
                    success = angdroid_obj_desc_append_formatted_detail(out,
                        cap, &end, " [%d,%+d]", 12, -4);
                    expected_len = strlen(" [12,-4]");
                    break;
                case 3:
                    expected = " (3 charges)";
                    success = angdroid_obj_desc_append_formatted_detail(out,
                        cap, &end, " (%d charge%s)", 3, "s");
                    expected_len = strlen(expected);
                    break;
                case 4:
                    expected = " (charging)";
                    success = angdroid_obj_desc_append_formatted_detail(out,
                        cap, &end, " (charging)");
                    expected_len = strlen(expected);
                    break;
                case 5:
                    expected = "（充能中）";
                    success = angdroid_obj_desc_append_formatted_detail(out,
                        cap, &end, "%s", expected);
                    expected_len = strlen(expected);
                    break;
                case 6:
                    expected = " <+12, -3, +4, -2147483648>";
                    success = angdroid_obj_desc_append_modifiers_ascii(out,
                        cap, &end, modifiers, 4);
                    expected_len = strlen(expected);
                    break;
                default:
                    expected = " <+12>";
                    success = angdroid_obj_desc_append_modifiers_ascii(out,
                        cap, &end, modifiers, 1);
                    expected_len = strlen(expected);
                    break;
                }
                CHECK(memory[0] == '#');
                CHECK(memory[cap + 1] == '#');
                CHECK(memory[159] == '#');
                if (!success) {
                    CHECK(end == offset);
                    CHECK(memcmp(memory, original, sizeof(memory)) == 0);
                } else {
                    CHECK(end == offset + expected_len);
                    CHECK(end < cap);
                    CHECK(out[end] == 0);
                    CHECK(memcmp(out, original + 1, offset) == 0);
                    if (mode != 0 && mode != 1 && mode != 2)
                        CHECK(memcmp(out + offset, expected, expected_len) == 0);
                }
            }
}

static void too_long(void)
{
    char buf[512] = "original";
    char large[200];
    int modifiers[64];
    size_t i, end = strlen(buf);
    memset(large, 'z', sizeof(large));
    large[sizeof(large)-1] = 0;
    for (i = 0; i < 64; ++i) modifiers[i] = INT_MIN;
    CHECK(!angdroid_obj_desc_append_formatted_detail(buf, sizeof(buf),
        &end, "%s", large));
    CHECK(!angdroid_obj_desc_append_modifiers_ascii(buf, sizeof(buf),
        &end, modifiers, 64));
    CHECK(strcmp(buf, "original") == 0);
    CHECK(end == strlen(buf));
}

int main(void)
{
    examples();
    capacities();
    too_long();
    printf("PASS: atomic numeric item details (%lu assertions)\n", assertions);
    return 0;
}
