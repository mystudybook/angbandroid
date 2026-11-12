/* The ignore-by-kind/ego menu must show complete bilingual labels. */
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/ignore-named-action-locale.h"

static unsigned long checked = 0;
#define CHECK(expr) do { checked++; if (!(expr)) { \
    fprintf(stderr, "FAIL:%d %s\n", __LINE__, #expr); return 1; \
} } while (0)

int main(void)
{
    const char *names[] = { "Long Sword", "屠龍長劍", "光芒（魔法）", "of Doriath" };
    const char *english_prefix[] = { "All ", "Unignore all " };
    const char *chinese_prefix[] = { "全部忽略 ", "取消忽略全部 " };
    char expected[256], buffer[256], tiny[5];
    size_t i, cap;
    int undo, zh;

    for (i = 0; i < sizeof(names) / sizeof(names[0]); i++) {
        for (undo = 0; undo <= 1; undo++) {
            for (zh = 0; zh <= 1; zh++) {
                const char *prefix = zh ? chinese_prefix[undo] :
                    english_prefix[undo];
                int n = snprintf(expected, sizeof(expected),
                    "%s%s", prefix, names[i]);
                CHECK(n > 0 && (size_t)n < sizeof(expected));
                for (cap = 0; cap <= sizeof(buffer) - 1; cap++) {
                    bool result;
                    memset(buffer, 'X', sizeof(buffer));
                    result = angdroid_ignore_named_action_text(
                        buffer, cap, names[i], undo, zh);
                    CHECK(buffer[sizeof(buffer) - 1] == 'X');
                    CHECK(result == (cap > strlen(expected)));
                    if (result) CHECK(strcmp(buffer, expected) == 0);
                    else if (cap > 0) CHECK(buffer[0] == '\0');
                }
            }
        }
    }

    memcpy(tiny, "abcd", sizeof(tiny));
    CHECK(!angdroid_ignore_named_action_text(tiny, 0, "a", 0, 1));
    CHECK(memcmp(tiny, "abcd", sizeof(tiny)) == 0);
    CHECK(!angdroid_ignore_named_action_text(tiny, sizeof(tiny), NULL, 0, 1));
    CHECK(tiny[0] == '\0');
    CHECK(!angdroid_ignore_named_action_text(tiny, sizeof(tiny), "", 0, 1));
    CHECK(tiny[0] == '\0');
    CHECK(!angdroid_ignore_named_action_text(NULL, 5, "a", 0, 1));

    printf("PASS: %lu bilingual ignore-name boundary assertions\n", checked);
    return 0;
}
